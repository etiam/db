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

#include <QScrollBar>
#include <QPainter>

#include "output.h"

namespace Ui
{

Output::Output(QWidget *parent) :
    QPlainTextEdit(parent)
{
    setObjectName("output");
    setReadOnly(true);

    setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    setCursorWidth(9);
}

void
Output::appendText(const QString &text)
{
    auto sanitized = text;
    sanitized.replace("\\n", "\n");
    sanitized.replace("\\t", "    ");

    // skip text with only a newline
    if (sanitized != "\n")
        insertText(sanitized);

    // scroll to bottom of text
    verticalScrollBar()->setValue(verticalScrollBar()->maximum());
}

void
Output::insertText(const QString &text)
{
    // jump to last known valid position
    auto cursor = textCursor();
    cursor.setPosition(m_cursorPos);
    setTextCursor(cursor);

    // insert the text
    insertPlainText(text);

    // update last known valid position
    m_cursorPos = textCursor().position();
}

}
