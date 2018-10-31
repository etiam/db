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

#include "core/global.h"
#include "core/signals.h"

#include "output.h"
#include "consoleInput.h"
#include "gdbConsole.h"

namespace Ui
{

GdbConsole::GdbConsole(QWidget *parent) :
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

    // connect signal handlers
    Core::Signals::debuggerStateUpdated.connect([this]()
    {
        QMetaObject::invokeMethod(this, "onDebuggerStateUpdated", Qt::QueuedConnection);
    });
}

void
GdbConsole::setTabFocus()
{
    m_lineedit->setFocus();
}

void
GdbConsole::appendText(const QString &text)
{
    m_output->appendText(text);
}

bool
GdbConsole::focusNextPrevChild(bool next)
{
    // prevent focus from leaving console input widget
    return false;
}

void
GdbConsole::onDebuggerStateUpdated()
{
    const auto state = Core::state()->debuggerState();

    switch(state)
    {
        case Core::State::Debugger::PAUSED:
        case Core::State::Debugger::LOADED:
            setDisabled(false);
            m_output->setDisabled(false);
            break;

        case Core::State::Debugger::RUNNING:
            setDisabled(true);
            m_output->setDisabled(true);
            break;

        default:
            break;
    }
}

}
