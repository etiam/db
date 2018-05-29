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

#include "debugControls.h"

namespace Ui
{

DebugControls::DebugControls(QWidget *parent) :
    QToolBar(parent)
{
    setObjectName(tr("debugcontrols"));
    setAllowedAreas(Qt::TopToolBarArea);

    QIcon runicon;
    runicon.addFile(":/img/run", QSize(32, 32), QIcon::Normal, QIcon::On);
    runicon.addFile(":/img/run-d", QSize(32, 32), QIcon::Disabled, QIcon::On);
    m_runAct = new QAction(runicon, tr("Run/Continue"), this);
    m_runAct->setStatusTip(tr("Start/continue debugging"));
    m_runAct->setShortcut(Qt::Key_R);
    connect(m_runAct, SIGNAL(triggered()), parent, SLOT(run()));
    addAction(m_runAct);

    QIcon pauseicon;
    pauseicon.addFile(":/img/pause", QSize(32, 32), QIcon::Normal, QIcon::On);
    pauseicon.addFile(":/img/pause-d", QSize(32, 32), QIcon::Disabled, QIcon::On);
    m_pauseAct = new QAction(pauseicon, tr("Pause"), this);
    m_pauseAct->setStatusTip(tr("Pause execution"));
    m_pauseAct->setShortcut(Qt::Key_P);
    connect(m_pauseAct, SIGNAL(triggered()), parent, SLOT(pause()));
    addAction(m_pauseAct);
    m_pauseAct->setDisabled(true);

    QIcon stopicon;
    stopicon.addFile(":/img/stop", QSize(32, 32), QIcon::Normal, QIcon::On);
    stopicon.addFile(":/img/stop-d", QSize(32, 32), QIcon::Disabled, QIcon::On);
    m_stopAct = new QAction(stopicon, tr("Stop"), this);
    m_stopAct->setStatusTip(tr("Stop execution"));
    connect(m_stopAct, SIGNAL(triggered()), parent, SLOT(stop()));
    addAction(m_stopAct);

    QIcon stepovericon;
    stepovericon.addFile(":/img/stepover", QSize(32, 32), QIcon::Normal, QIcon::On);
    stepovericon.addFile(":/img/stepover-d", QSize(32, 32), QIcon::Disabled, QIcon::On);
    m_stepoverAct = new QAction(stepovericon, tr("Step over"), this);
    m_stepoverAct->setStatusTip(tr("Step over"));
    m_stepoverAct->setShortcut(Qt::Key_N);
    connect(m_stepoverAct, SIGNAL(triggered()), parent, SLOT(stepover()));
    addAction(m_stepoverAct);

    QIcon stepintoicon;
    stepintoicon.addFile(":/img/stepinto", QSize(32, 32), QIcon::Normal, QIcon::On);
    stepintoicon.addFile(":/img/stepinto-d", QSize(32, 32), QIcon::Disabled, QIcon::On);
    m_stepintoAct = new QAction(stepintoicon, tr("Step into"), this);
    m_stepintoAct->setStatusTip(tr("Step into"));
    m_stepintoAct->setShortcut(Qt::Key_S);
    connect(m_stepintoAct, SIGNAL(triggered()), parent, SLOT(stepinto()));
    addAction(m_stepintoAct);

    QIcon stepouticon;
    stepouticon.addFile(":/img/stepout", QSize(32, 32), QIcon::Normal, QIcon::On);
    stepouticon.addFile(":/img/stepout-d", QSize(32, 32), QIcon::Disabled, QIcon::On);
    m_stepoutAct = new QAction(stepouticon, tr("Step out"), this);
    m_stepoutAct->setStatusTip(tr("Step out"));
    connect(m_stepoutAct, SIGNAL(triggered()), parent, SLOT(stepout()));
    addAction(m_stepoutAct);
}

DebugControls::~DebugControls()
{
}

void
DebugControls::setState(Core::State::Debugger state)
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
            break;

        case Core::State::Debugger::RUNNING:
            m_pauseAct->setDisabled(false);
            m_stopAct->setDisabled(false);
            break;

        case Core::State::Debugger::PAUSED:
            m_runAct->setDisabled(false);
            m_stopAct->setDisabled(false);
            m_stepoverAct->setDisabled(false);
            m_stepintoAct->setDisabled(false);
            m_stepoutAct->setDisabled(false);
            break;

        default:
            break;
    }
}

} // namespace Ui
