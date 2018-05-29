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

class DebugControls : public QToolBar
{
  public:
    DebugControls(QWidget *parent=0);
    ~DebugControls();

    void setState(Core::State::Debugger state);

  private:
    QAction * m_runAct;
    QAction * m_pauseAct;
    QAction * m_stopAct;
    QAction * m_stepoverAct;
    QAction * m_stepintoAct;
    QAction * m_stepoutAct;
};

} // namespace Ui

#endif // UI_DEBUGCONTROLS_H_
