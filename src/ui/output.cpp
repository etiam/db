/*
 * output.cpp
 *
 *  Created on: Jun 17, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <iostream>
#include <thread>

#include "core/global.h"
#include "core/state.h"
#include "core/signals.h"

#include <QScrollBar>
#include <QFontDatabase>
#include <QTimer>

#include "output.h"

namespace Ui
{

Output::Output(QWidget *parent) :
    QTextEdit(parent)
{
    setObjectName("output");
    setReadOnly(true);

    setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    setCursorWidth(9);

    m_buffer.reserve(m_bufferSize);

    // TODO : maybe combine these two timers

    // flush the output buffer every 0.5sec
    m_flushTimer = new QTimer(this);
    m_flushTimer->setInterval(500);
    m_flushTimer->start();
    connect(m_flushTimer, &QTimer::timeout, [this]() { flush();});

    // trim the scrollback buffer every 0.5sec
    m_trimTimer = new QTimer(this);
    m_trimTimer->setInterval(500);
    m_trimTimer->start();
    connect(m_trimTimer, &QTimer::timeout, [this]() { trim();});

    // connect signal handlers
    Core::Signals::debuggerStateUpdated.connect(this, &Output::onDebuggerStateUpdated);
}

void
Output::setTabFocus()
{
    setFocus();
}

void
Output::appendText(const QString &text)
{
    if (text.length() > m_bufferSize - m_buffer.length())
    {
        flush();
    }

    m_buffer += text;
}

void
Output::flush()
{
    if (m_buffer.size() > 0)
    {
        auto sanitized = m_buffer;
        sanitized.replace("\\n", "\n");
        sanitized.replace("\\t", "    ");
        sanitized.replace("\\\"", "\"");

        // don't insert text with only a newline
        if (sanitized != "\n")
        {
            moveCursor (QTextCursor::End);
            insertPlainText(sanitized);
            moveCursor (QTextCursor::End);
        }

        // scroll to bottom of text
        verticalScrollBar()->setValue(verticalScrollBar()->maximum());

        m_buffer.clear();
    }
}

void
Output::trim()
{
    auto tmp = toPlainText();
    auto delta = tmp.size() - static_cast<int>(m_maxScrollbackSize);
    if (delta > 0)
    {
        // remove number of bytes over m_maxScrollbackSize
        tmp.remove(0, tmp.indexOf("\n", delta) + 1);
        setPlainText(tmp);

        // scroll to bottom of text
        verticalScrollBar()->setValue(verticalScrollBar()->maximum());
    }
}

void
Output::onDebuggerStateUpdated()
{
    const auto state = Core::state()->debuggerState();

    switch(state)
    {
        case Core::State::Debugger::PAUSED:
        case Core::State::Debugger::LOADED:
            setDisabled(false);
            break;

        case Core::State::Debugger::RUNNING:
            setDisabled(true);
            break;

        default:
            break;
    }
}

}
