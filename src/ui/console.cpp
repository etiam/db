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

#include <QApplication>
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

    if (sanitized != "\n")
        insertPlainText(sanitized);

    // scroll to bottom of text
    verticalScrollBar()->setValue(verticalScrollBar()->maximum());

    m_consoleUpdateTimer.start();
    m_promptDisplayed = false;
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
            insertPlainText("\n");
//            insertPlainText("\n(gdb) ");
            verticalScrollBar()->setValue(verticalScrollBar()->maximum());
            m_consoleUpdateTimer.start();
            m_promptDisplayed = false;

            break;

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
    if (!m_promptDisplayed && m_consoleUpdateTimer.elapsed() > 100)
    {
        insertPlainText("(gdb) ");
        verticalScrollBar()->setValue(verticalScrollBar()->maximum());
        m_promptDisplayed = true;
    }
}

}
