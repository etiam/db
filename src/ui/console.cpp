/*
 * console.cpp
 *
 *  Created on: Apr 12, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "console.h"

namespace Ui
{

Console::Console(QWidget *parent) :
    QPlainTextEdit(parent)
{
    setReadOnly(true);
    setFont(QFont("hack", 10));
}

Console::~Console()
{
}

void
Console::appendText(const QString &text, bool newline)
{
    insertPlainText(text);
    if (newline)
    {
        moveCursor(QTextCursor::Up);
        moveCursor(QTextCursor::EndOfLine);
    }
}

}
