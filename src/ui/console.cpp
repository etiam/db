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

#include <QVBoxLayout>
#include <QProxyStyle>
#include <QCompleter>
#include <QTimer>
#include <QDebug>
#include <QStandardItemModel>

#include "core/global.h"
#include "core/signal.h"
#include "gdb/commands.h"

#include "external/lineedit/src/history_line_edit.hpp"

#include "output.h"
#include "console.h"

namespace
{

class ConsoleInput : public HistoryLineEdit
{
  public:
    explicit ConsoleInput(QWidget *parent = nullptr);

  protected:
    void keyPressEvent(QKeyEvent *) override;
    void focusInEvent(QFocusEvent *event) override;

  private:
    void autoComplete(bool notify);
    void onCompletionDataUpdated();

    QCompleter *m_completer;
};

// custom block style cursor
class ConsoleInputStyle : public QProxyStyle
{
  public:
    ConsoleInputStyle(QStyle *style = nullptr);

    int pixelMetric(PixelMetric metric, const QStyleOption *option = nullptr, const QWidget *widget = nullptr) const;
    int m_fontWidth;
};

ConsoleInput::ConsoleInput(QWidget *parent) :
    HistoryLineEdit(parent)
{
    setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    setStyle(new ConsoleInputStyle(style()));

    setText("(gdb) ");

    // build completion text model
    auto model = new QStandardItemModel(0, 2, this);

    // insert gdb commands in column 0
    QStringList gdbcommands;
    gdbcommands << "break" << "next" << "step" << "return" << "enable" << "disable" << "quit";
    gdbcommands << "add-auto-load-safe-path" << "add-inferior" << "add-symbol-file-from-memory" << "add-auto-load-scripts-directory  add-symbol-file";
    for (const auto &word : gdbcommands)
    {
        auto rowcount = model->rowCount();
        model->insertRow(rowcount);

        model->setData(model->index(rowcount, 0), word);
    }

    // create and assign completer
    m_completer = new QCompleter(model, this);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_completer->setCompletionMode(QCompleter::InlineCompletion);
    m_completer->setCompletionColumn(0);

    Core::Signal::completionDataUpdated.connect(this, &ConsoleInput::onCompletionDataUpdated);
}

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
            if (current_word().length() > 0)
                HistoryLineEdit::keyPressEvent(event);
            setText("(gdb) ");
            break;

        case Qt::Key_Down:
            HistoryLineEdit::keyPressEvent(event);
            if (current_word().length() == 0)
                setText("(gdb) ");
            break;

        case Qt::Key_Escape:
            for (const auto &h : history())
                std::cout << h.toStdString() << std::endl;
            break;

        case Qt::Key_Backtab:
            break;

        case Qt::Key_Tab:
        {
            auto notify =  (lastkey == Qt::Key_Tab);
            autoComplete(notify);
            break;
        }

        default:
            HistoryLineEdit::keyPressEvent(event);
    }

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
    std::cout << std::boolalpha << notify << std::endl;

    // set completion text, stripping out prompt
    m_completer->setCompletionPrefix(text().mid(6));

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
        Core::Signal::appendConsoleText(matches.join(", ").toStdString());
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

            // more than one, find common prefix and use that
            else
            {
                auto first = matches[0];
                auto second = matches[1];
                for (int n=0; n < first.size(); ++n)
                {
                    if (first[n] != second[n])
                    {
                        completion = first.mid(0, n);
                        break;
                    }
                }
            }

            // update text()
            if (completion.size() > 0)
            {
                QString newtext = text() + completion.mid(m_completer->completionPrefix().size());

                // there was only a single match, so insert space after as it's a complete word
                if (matches.size() == 1)
                    newtext += " ";

                setText(newtext);
            }
        }
    }
}

void
ConsoleInput::onCompletionDataUpdated()
{
    auto model = m_completer->model();

    // insert source files into column 1 of m_completer's model
    const auto &sourcefiles = Core::state()->sourceFiles();
    for (const auto &filename : sourcefiles)
    {
        auto rowcount = model->rowCount();
        model->insertRow(rowcount);

        model->setData(model->index(rowcount, 1), QString::fromStdString(filename));
    }
}

}

namespace Ui
{

Console::Console(QWidget *parent, bool editable) :
    QFrame(parent)
{
    setObjectName("console");

    m_output = new Output(this);
    m_output->setObjectName("consoleoutput");

    m_lineedit = new ConsoleInput(this);
    m_lineedit->setObjectName("consolelineedit");

    // default focus goes to console input widget, use a timer
    // because other (not yet created) widgets might take focus
    // when created
    QTimer::singleShot(0, [this]() { m_lineedit->setFocus(); });

    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_output);
    layout->addWidget(m_lineedit);

    setLayout(layout);
}

void
Console::appendText(const QString &text)
{
    m_output->appendText(text);
}

bool
Console::focusNextPrevChild(bool next)
{
    // prevent focus from leaving console input widget
    return false;
}

}
