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
    void autoComplete();

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
    for (const auto &word : gdbcommands)
    {
        auto rowcount = model->rowCount();
        model->insertRow(rowcount);

        model->setData(model->index(rowcount, 0), word);
    }

    // insert source files in column 1
    const auto &sourcefiles = Core::state()->sourceFiles();
    for (const auto &filename : sourcefiles)
    {
        auto rowcount = model->rowCount();
        model->insertRow(rowcount);

        model->setData(model->index(rowcount, 1), QString::fromStdString(filename));
    }

    m_completer = new QCompleter(model, this);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_completer->setCompletionMode(QCompleter::InlineCompletion);
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
            autoComplete();
            break;

        default:
            HistoryLineEdit::keyPressEvent(event);
    }
}

void
ConsoleInput::focusInEvent(QFocusEvent *event)
{
    HistoryLineEdit::focusInEvent(event);

    // prevent selected text on focusin event
    deselect();
}

void
ConsoleInput::autoComplete()
{
    // set completion text, stripping out prompt
    m_completer->setCompletionPrefix(text().mid(6));

    for (int i = 0; m_completer->setCurrentRow(i); i++)
        qDebug() << m_completer->currentCompletion() << " is match number " << i;

//    auto index = m_completer->currentIndex();
//    auto start = m_completer->currentRow();
//def next_completion(self):
//    index = self._compl.currentIndex()
//    self._compl.popup().setCurrentIndex(index)
//    start = self._compl.currentRow()
//    if not self._compl.setCurrentRow(start + 1):
//        self._compl.setCurrentRow(0)
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
