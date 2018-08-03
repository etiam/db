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

  private:
    std::unique_ptr<QStyle> m_style;
};

class LineEditStyle : public QProxyStyle
{
  public:
    LineEditStyle(QStyle *style = 0) : QProxyStyle(style) { }

    int pixelMetric(PixelMetric metric, const QStyleOption *option = 0, const QWidget *widget = 0) const;
};

ConsoleInput::ConsoleInput(QWidget *parent) :
    HistoryLineEdit(parent)
{
    setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    m_style = std::make_unique<LineEditStyle>(style());
    setStyle(m_style.get());

    setText("(gdb) ");
}

int
LineEditStyle::pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
    if (metric == QStyle::PM_TextCursorWidth)
        return 7;

    return QProxyStyle::pixelMetric(metric, option, widget);
}


void ConsoleInput::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
        case Qt::Key_Backspace:
            if (cursorPosition() < 7)
                break;

        default:
            HistoryLineEdit::keyPressEvent(event);
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

}
