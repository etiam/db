/*
 * console.cpp
 *
 *  Created on: Apr 12, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <iostream>

#include <boost/filesystem/operations.hpp>

#include <QVBoxLayout>
#include <QProxyStyle>
#include <QCompleter>
#include <QStandardItemModel>
#include <QFontDatabase>
#include <QKeyEvent>

#include "core/global.h"
#include "core/signals.h"
#include "gdb/commands.h"
#include "ast/builder.h"

#include "consoleInput.h"

namespace
{

// custom block style cursor
class ConsoleInputStyle : public QProxyStyle
{
  public:
    ConsoleInputStyle(QStyle *style = nullptr);

    int pixelMetric(PixelMetric metric, const QStyleOption *option = nullptr, const QWidget *widget = nullptr) const;
    int m_fontWidth;
};

ConsoleInputStyle::ConsoleInputStyle(QStyle *style) : QProxyStyle(style)
{
    // store character width
    const auto &font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    m_fontWidth = QFontMetrics(font).width(" ");
}

int
ConsoleInputStyle::pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
    // block cursor
    if (metric == QStyle::PM_TextCursorWidth)
        return m_fontWidth;

    return QProxyStyle::pixelMetric(metric, option, widget);
}

bool
commonPrefix(const QStringList &strings, QString &prefix)
{
    const char *b;
    int n;
    std::ptrdiff_t pos, minpos;

    const auto a = qstrdup(strings[0].toLatin1().constData());

    const auto strlena = std::strlen(a);
    for (n=1; n < strings.size(); ++n)
    {
        b = strings[n].toLatin1().constData();
        if (std::strlen(b) <= strlena)
            pos = std::distance(b, std::mismatch(b, b + std::strlen(b), a).first);
        else
            pos = std::distance(a, std::mismatch(a, a + std::strlen(a), b).first);

        std::cout << n << " " << a << " " << b << " " << pos << " " << minpos << std::endl;

        minpos = std::min(minpos, pos);
        if (n > 1 && pos == 0)
            break;
    }

    prefix = strings[0].mid(0, minpos);

    return minpos != 0;
}

}

namespace Ui
{

ConsoleInput::ConsoleInput(QWidget *parent) :
    HistoryLineEdit(parent)
{
    setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    setStyle(new ConsoleInputStyle(style()));

    setText("(gdb) ");

    // create and assign completer
    m_completer = new QCompleter(this);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_completer->setCompletionMode(QCompleter::InlineCompletion);
    m_completer->setCompletionColumn(0);

    updateCompletionData();

    Core::Signals::sourceListUpdated.connect([this]()
    {
        QMetaObject::invokeMethod(this, "updateCompletionData", Qt::QueuedConnection);
    });

    Core::Signals::functionListUpdated.connect([this]()
    {
        QMetaObject::invokeMethod(this, "updateCompletionData", Qt::QueuedConnection);
    });
}

void
ConsoleInput::keyPressEvent(QKeyEvent *event)
{
    static int lastkey;

    switch (event->key())
    {
        case Qt::Key_Backspace:
        case Qt::Key_Left:
            if (cursorPosition() >= 7)
                HistoryLineEdit::keyPressEvent(event);
            break;

        case Qt::Key_Enter:
        case Qt::Key_Return:
            if (text().mid(6) > 0)
                HistoryLineEdit::keyPressEvent(event);
            setText("(gdb) ");
            break;

        case Qt::Key_Down:
            HistoryLineEdit::keyPressEvent(event);
            if (current_word().length() == 0)
                setText("(gdb) ");
            break;

        // ignore backtab
        case Qt::Key_Backtab:
            break;

        // run autocomplete
        case Qt::Key_Tab:
            autoComplete(lastkey == Qt::Key_Tab);
            break;

        default:
            HistoryLineEdit::keyPressEvent(event);
    }

    // keep last key for double tab key action
    lastkey = event->key();
}

void
ConsoleInput::focusInEvent(QFocusEvent *event)
{
    HistoryLineEdit::focusInEvent(event);

    // prevent the auto-selecting of text on focusin event
    deselect();
}

void
ConsoleInput::autoComplete(bool notify)
{
    // get input text, stripping out prompt
    auto thetext = text().mid(6);

    int firstspace = thetext.leftRef(cursorPosition()).indexOf(' ') + 1;
    int lastspace = thetext.leftRef(cursorPosition()).lastIndexOf(' ') + 1;
    auto firstword = thetext.mid(0, firstspace);
    auto lastword = thetext.mid(lastspace);

    // determine which column to auto complete from
    if (firstword != "break ")
        m_completer->setCompletionColumn(0);
    else
        m_completer->setCompletionColumn(1);

    // set completion prefix to last word
    m_completer->setCompletionPrefix(lastword);

    // make string list of potential matches
    QStringList matches;
    for (int i = 0; m_completer->setCurrentRow(i); i++)
    {
        const auto &string = m_completer->currentCompletion();
        if (string.size() > 0)
        {
            matches << string;
        }
    }

    // if notify, just send list of matches to console
    if (notify)
    {
        Core::Signals::appendConsoleText(matches.join(", ").toStdString() + "\n");
    }

    // otherwise try to complete
    else
    {
        if (matches.size() > 0)
        {
            QString completion;

            // only one match, use that
            if (matches.size() == 1)
            {
                completion = matches[0];
            }

            // more than one, if there is a common prefix and use that
            else
            {
                QString prefix;
                if (commonPrefix(matches, prefix))
                {
                    completion = prefix;
//                    std::cout << prefix.toStdString() << std::endl;
//                    auto first = matches[0];
//                    auto second = matches[1];
//                    for (int n=0; n < first.size(); ++n)
//                    {
//                        if (first[n] != second[n])
//                        {
//                            completion = first.mid(0, n);
//                            break;
//                        }
//                    }
                }
            }

            // update text()
            if (completion.size() > 0)
            {
                QString newtext = text() + completion.mid(m_completer->completionPrefix().size());
                setText(newtext);
            }
        }
    }
}

void
ConsoleInput::updateCompletionData()
{
    // create and assign new model
    auto model = new QStandardItemModel(0, 2, this);
    m_completer->setModel(model);

    // insert gdb commands in column 0 of model
    QStringList gdbcommands;
    gdbcommands << "break" << "next" << "step" << "return" << "enable" << "disable" << "quit";
    for (const auto &word : gdbcommands)
    {
        auto rowcount = model->rowCount();
        model->insertRow(rowcount);

        model->setData(model->index(rowcount, 0), word);
    }

    // insert source file names into column of model
    const auto &sourcefiles = Core::state()->sourceFiles();
    std::vector<std::string> filenames;
    for (const auto &fullname : sourcefiles)
    {
        const auto filename = boost::filesystem::path(fullname).filename().string();
        if (std::find(std::begin(filenames), std::end(filenames), filename) == std::end(filenames))
        {
            filenames.push_back(filename);
            auto rowcount = model->rowCount();
            model->insertRow(rowcount);
            model->setData(model->index(rowcount, 1), QString::fromStdString(filename));
        }
    }
    std::cout << "inserted " << filenames.size() << " filename(s) into completion model" << std::endl;

    // insert source function names into column of model
    std::vector<std::string> funcnames;
    const auto &functions = Core::ast()->functions();
    for (const auto ref : functions)
    {
        const auto &funcname = ref.second.spelling;

        // ignore reserved functions
        if (funcname[0] == '_')
            continue;

        if (std::find(std::begin(funcnames), std::end(funcnames), funcname) == std::end(funcnames))
        {
            funcnames.push_back(funcname);
            auto rowcount = model->rowCount();
            model->insertRow(rowcount);
            model->setData(model->index(rowcount, 1), QString::fromStdString(funcname));
        }
    }
    std::cout << "inserted " << funcnames.size() << " funcname(s) into completion model" << std::endl;

    std::cout << model->rowCount() << std::endl;
}

}
