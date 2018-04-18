/*
 * console.cpp
 *
 *  Created on: Apr 12, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

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
Console::appendText(const QString &text, bool newline)
{
    insertPlainText(text);
    if (newline)
    {
        moveCursor(QTextCursor::Up);
        moveCursor(QTextCursor::EndOfLine);
    }
}

// wink signal handlers

void
Console::onAppendConsoleTextSignal(const std::string &text, bool newline)
{
    QMetaObject::invokeMethod(this, "appendText", Qt::QueuedConnection, Q_ARG(QString, QString::fromStdString(text)),
                                                                        Q_ARG(bool, newline));
}

}
