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

#include <QScrollBar>
#include <QPainter>

#include "core/signal.h"

#include "console.h"

namespace Ui
{

Console::Console(QWidget *parent, bool editable) :
    QPlainTextEdit(parent),
    m_editable(editable)
{
    if (!m_editable)
        setReadOnly(true);

    setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    setCursorWidth(7);
}

Console::~Console()
{
}

void
Console::appendText(const QString &text)
{
    auto sanitized = text;
    sanitized.replace("\\n", "\n");
    sanitized.replace("\\t", "    ");

    insertPlainText(sanitized);

//    if (!sanitized.endsWith('\n'))
//    {
//        moveCursor(QTextCursor::Up);
//        moveCursor(QTextCursor::EndOfLine);
//    }

    // scroll to bottom of text
    verticalScrollBar()->setValue(verticalScrollBar()->maximum());
}

void Console::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
        case Qt::Key_Backspace:
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_Up:
        case Qt::Key_Down:
            break;

        default:
            if (m_editable)
                QPlainTextEdit::keyPressEvent(e);
    }
}

}
