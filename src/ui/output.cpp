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

        // skip text with only a newline
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

}
