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
#include <QTimer>

#include "core/signal.h"

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
        insertPlainText(text);

    // scroll to bottom of text
    verticalScrollBar()->setValue(verticalScrollBar()->maximum());
}

}
