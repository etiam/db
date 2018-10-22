/*
 * callStack.h
 *
 *  Created on: Jul 21, 2018
 *      Author: jasonr
 */

#ifndef SRC_UI_BREAKPOINTS_H_
#define SRC_UI_BREAKPOINTS_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <QTreeView>

class QStandardItemModel;

namespace Ui
{

class BreakPointsItemModel;

class BreakPoints: public QTreeView
{
Q_OBJECT

public:
    BreakPoints(QWidget *parent = nullptr);
    ~BreakPoints() = default;

protected:
    void mouseDoubleClickEvent(QMouseEvent *) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    BreakPointsItemModel * m_model;

private Q_SLOTS:
    void onBreakPointsUpdated();
    void onDebuggerStateUpdated();
};

} // namespace Ui

#endif // SRC_UI_BREAKPOINTS_H_
