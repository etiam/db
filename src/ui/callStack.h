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

class CallStackItemModel;

class CallStack : public QTreeView
{
  Q_OBJECT

public:
    CallStack(QWidget *parent = nullptr);
    ~CallStack() = default;

    void onCallStackUpdated();

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void loadSourceAtRow(int row);

    CallStackItemModel * m_model;
};

} // namespace Ui

#endif // SRC_UI_CALLSTACK_H_
