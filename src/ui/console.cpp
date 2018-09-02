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
#include <QTimer>

#include "output.h"
#include "consoleInput.h"
#include "console.h"

namespace Ui
{

Console::Console(QWidget *parent, bool editable) :
    QFrame(parent)
{
    setObjectName("gdb");

    m_output = new Output(this);
    m_output->setObjectName("consoleoutput");

    m_lineedit = new ConsoleInput(this);
    m_lineedit->setObjectName("consolelineedit");

    // default focus goes to console input widget, use a timer
    // because other (not yet created) widgets might take focus
    // when created
    QTimer::singleShot(0, [this]() { m_lineedit->setFocus(); });

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

bool
Console::focusNextPrevChild(bool next)
{
    // prevent focus from leaving console input widget
    return false;
}

}
