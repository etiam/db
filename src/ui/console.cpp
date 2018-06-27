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
#include <QTimer>

#include "core/signal.h"

#include "console.h"

namespace Ui
{

Console::Console(QWidget *parent, bool editable) :
    QPlainTextEdit(parent),
    m_editable(editable)
{
    setObjectName("console");
    if (!m_editable)
        setReadOnly(true);

    setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    setCursorWidth(9);

    auto prompttimer = new QTimer(this);
    connect(prompttimer, SIGNAL(timeout()), this, SLOT(showPrompt()));
    prompttimer->setInterval(100);
    prompttimer->start();
}

void
Console::appendText(const QString &text)
{
    auto sanitized = text;
    sanitized.replace("\\n", "\n");
    sanitized.replace("\\t", "    ");

    // skip text with only a newline
    if (sanitized != "\n")
        insertText(sanitized);

    // scroll to bottom of text
    verticalScrollBar()->setValue(verticalScrollBar()->maximum());

    // reset prompt timer
    m_consoleUpdateTimer.start();

    // only display prompt after newline
    if(sanitized.endsWith("\n"))
        m_showPrompt = true;
}

void
Console::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
        case Qt::Key_Up:
        case Qt::Key_Down:
            break;

        case Qt::Key_Return:
        {
            insertText("\n");

            // scroll to bottom of text
            verticalScrollBar()->setValue(verticalScrollBar()->maximum());

            // reset prompt timer
            m_consoleUpdateTimer.start();

            m_showPrompt = true;
            break;
        }

        default:
            if (m_editable)
                QPlainTextEdit::keyPressEvent(e);
    }
}


void
Console::mousePressEvent(QMouseEvent* e)
{
}

void
Console::showPrompt()
{
    if (m_showPrompt && m_consoleUpdateTimer.elapsed() > 100)
    {
        insertText("(gdb) ");

        // scroll to bottom of text
        verticalScrollBar()->setValue(verticalScrollBar()->maximum());

        // don't show prompt again until new text added
        m_showPrompt = false;
    }
}

void
Console::insertText(const QString &text)
{
    // jump to last known valid position
    auto cursor = textCursor();
    cursor.setPosition(m_textCursorPos);
    setTextCursor(cursor);

    // insert the text
    insertPlainText(text);

    // update last known valid position
    m_textCursorPos = textCursor().position();
}

}
