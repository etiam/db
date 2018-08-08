/*
 * debugControls.cpp
 *
 *  Created on: May 15, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <QIcon>
#include <QAction>

#include "core/global.h"
#include "core/signals.h"

#include "gdb/commands.h"

#include "mainWindow.h"
#include "debugControls.h"

namespace Ui
{

DebugControls::DebugControls(MainWindow *parent) :
    QToolBar(parent)
{
    setObjectName("debugcontrols");
    setAllowedAreas(Qt::TopToolBarArea);

    QIcon runicon;
    runicon.addFile(":/img/run", QSize(32, 32), QIcon::Normal, QIcon::On);
    runicon.addFile(":/img/run-d", QSize(32, 32), QIcon::Disabled, QIcon::On);
    m_runAct = new QAction(runicon, tr("Run"), this);
    m_runAct->setStatusTip(tr("Start execution"));
    m_runAct->setShortcut(Qt::Key_R);
    connect(m_runAct, &QAction::triggered, [&]() { run(); } );
    addAction(m_runAct);

    QIcon pauseicon;
    pauseicon.addFile(":/img/pause", QSize(32, 32), QIcon::Normal, QIcon::On);
    pauseicon.addFile(":/img/pause-d", QSize(32, 32), QIcon::Disabled, QIcon::On);
    m_pauseAct = new QAction(pauseicon, tr("Pause"), this);
    m_pauseAct->setStatusTip(tr("Pause execution"));
    m_pauseAct->setShortcut(Qt::Key_P);
    connect(m_pauseAct, &QAction::triggered, [&](){ Core::gdb()->pause(); });
    addAction(m_pauseAct);

    QIcon stopicon;
    stopicon.addFile(":/img/stop", QSize(32, 32), QIcon::Normal, QIcon::On);
    stopicon.addFile(":/img/stop-d", QSize(32, 32), QIcon::Disabled, QIcon::On);
    m_stopAct = new QAction(stopicon, tr("Stop"), this);
    m_stopAct->setStatusTip(tr("Stop execution"));
    connect(m_stopAct, &QAction::triggered, [&](){ Core::gdb()->stop(); });
    addAction(m_stopAct);

    QIcon stepovericon;
    stepovericon.addFile(":/img/stepover", QSize(32, 32), QIcon::Normal, QIcon::On);
    stepovericon.addFile(":/img/stepover-d", QSize(32, 32), QIcon::Disabled, QIcon::On);
    m_stepoverAct = new QAction(stepovericon, tr("Step over"), this);
    m_stepoverAct->setStatusTip(tr("Step over"));
    m_stepoverAct->setShortcut(Qt::Key_N);
    connect(m_stepoverAct, &QAction::triggered, [&](){ Core::gdb()->stepover(); });
    addAction(m_stepoverAct);

    QIcon stepintoicon;
    stepintoicon.addFile(":/img/stepinto", QSize(32, 32), QIcon::Normal, QIcon::On);
    stepintoicon.addFile(":/img/stepinto-d", QSize(32, 32), QIcon::Disabled, QIcon::On);
    m_stepintoAct = new QAction(stepintoicon, tr("Step into"), this);
    m_stepintoAct->setStatusTip(tr("Step into"));
    m_stepintoAct->setShortcut(Qt::Key_S);
    connect(m_stepintoAct, &QAction::triggered, [&](){ Core::gdb()->stepinto(); });
    addAction(m_stepintoAct);

    QIcon stepouticon;
    stepouticon.addFile(":/img/stepout", QSize(32, 32), QIcon::Normal, QIcon::On);
    stepouticon.addFile(":/img/stepout-d", QSize(32, 32), QIcon::Disabled, QIcon::On);
    m_stepoutAct = new QAction(stepouticon, tr("Step out"), this);
    m_stepoutAct->setStatusTip(tr("Step out"));
    connect(m_stepoutAct, &QAction::triggered, [&](){ Core::gdb()->stepout(); });
    addAction(m_stepoutAct);
}

DebugControls::~DebugControls()
{
}

void
DebugControls::updateActionsState(Core::State::Debugger state)
{
    m_runAct->setDisabled(true);
    m_pauseAct->setDisabled(true);
    m_stopAct->setDisabled(true);
    m_stepoverAct->setDisabled(true);
    m_stepintoAct->setDisabled(true);
    m_stepoutAct->setDisabled(true);

    switch(state)
    {
        case Core::State::Debugger::LOADED:
            m_runAct->setDisabled(false);

            m_runAct->setStatusTip(tr("Start execution"));
            m_runAct->setText(tr("Run"));
            break;

        case Core::State::Debugger::RUNNING:
            m_pauseAct->setDisabled(false);
            m_stopAct->setDisabled(false);

            m_runAct->setStatusTip(tr("Start execution"));
            m_runAct->setText(tr("Run"));
            break;

        case Core::State::Debugger::PAUSED:
            m_runAct->setDisabled(false);
            m_stopAct->setDisabled(false);
            m_stepoverAct->setDisabled(false);
            m_stepintoAct->setDisabled(false);
            m_stepoutAct->setDisabled(false);

            m_runAct->setStatusTip(tr("Continue execution"));
            m_runAct->setText(tr("Continue"));
            break;

        default:
            break;
    }
}

void
DebugControls::run()
{
    // this will make the current location marker disappear
    Core::Signals::clearCurrentLocation();

    if (Core::state()->debuggerState() == Core::State::Debugger::PAUSED)
        Core::gdb()->cont();
    else
        Core::gdb()->run();
}

} // namespace Ui
