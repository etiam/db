/*
 * callStack.h
 *
 *  Created on: Jun 6, 2018
 *      Author: jasonr
 */

#ifndef SRC_UI_CALLSTACK_H_
#define SRC_UI_CALLSTACK_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <QTreeView>

class QStandardItemModel;

namespace Ui
{

class CallStack : public QTreeView
{
  Q_OBJECT

  public:
    CallStack(QWidget *parent = nullptr);
    ~CallStack() = default;

    void            onCallStackUpdated();

  private:
    QStandardItemModel *    m_model;
};

} // namespace Ui

#endif // SRC_UI_CALLSTACK_H_
