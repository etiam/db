/*
 * console.cpp
 *
 *  Created on: Apr 12, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <QVBoxLayout>

#include "core/global.h"
#include "core/signal.h"
#include "gdb/commands.h"

#include "external/lineedit/src/history_line_edit.hpp"

#include "output.h"
#include "console.h"

namespace Ui
{

Console::Console(QWidget *parent, bool editable) :
    QFrame(parent)
{
    setObjectName("console");

    m_output = new Output(this);
    m_output->setObjectName("consoleoutput");

    m_lineedit = new HistoryLineEdit(this);
    m_lineedit->setObjectName("consolelineedit");

    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_output);
    layout->addWidget(m_lineedit);

    setLayout(layout);
}

void
Console::appendText(const QString &text)
{
    m_output->appendText(text);
}

}
