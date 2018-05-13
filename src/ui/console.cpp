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

#include "core/signal.h"

#include "console.h"

namespace Ui
{

Console::Console(QWidget *parent) :
    QPlainTextEdit(parent)
{
    setReadOnly(true);
    setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    auto f = QFontDatabase::systemFont(QFontDatabase::FixedFont);
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

    if (!sanitized.endsWith('\n'))
    {
        moveCursor(QTextCursor::Up);
        moveCursor(QTextCursor::EndOfLine);
    }

    // scroll to bottom of text
    verticalScrollBar()->setValue(verticalScrollBar()->maximum());
}

}
