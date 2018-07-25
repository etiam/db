/*
 * debugControls.h
 *
 *  Created on: May 15, 2018
 *      Author: jasonr
 */

#ifndef UI_DEBUGCONTROLS_H_
#define UI_DEBUGCONTROLS_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <QToolBar>

#include "core/state.h"

namespace Ui
{

class MainWindow;

class DebugControls : public QToolBar
{
  public:
    DebugControls(MainWindow *parent=0);
    ~DebugControls();

    // update the enabled/disabled state of the debug controls
    void updateActionsState(Core::State::Debugger state);

  private:
    void run();

    QAction * m_runAct;
    QAction * m_pauseAct;
    QAction * m_stopAct;
    QAction * m_stepoverAct;
    QAction * m_stepintoAct;
    QAction * m_stepoutAct;
};

} // namespace Ui

#endif // UI_DEBUGCONTROLS_H_
