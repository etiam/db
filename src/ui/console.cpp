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

#include "core/signal.h"

#include "console.h"

namespace Ui
{

Console::Console(QWidget *parent) :
    QPlainTextEdit(parent)
{
    setReadOnly(true);
    setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    Core::appendConsoleTextSignal.connect(this, &Console::onAppendConsoleTextSignal);
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
}

// wink signal handlers

void
Console::onAppendConsoleTextSignal(const std::string &text)
{
    QMetaObject::invokeMethod(this, "appendText", Qt::QueuedConnection, Q_ARG(QString, QString::fromStdString(text)));
}

}
