/*
 * variables.h
 *
 *  Created on: Oct 22, 2018
 *      Author: jasonr
 */

#ifndef SRC_UI_VARIABLES_H_
#define SRC_UI_VARIABLES_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <QTreeView>

#include "tabPage.h"

namespace Ui
{

class VariablesItemModel;

class Variables : public QTreeView, public TabPage
{
  Q_OBJECT

public:
    explicit Variables(QWidget *parent = nullptr);
    ~Variables() = default;

    void setTabFocus() override;

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    VariablesItemModel * m_model;

private Q_SLOTS:
    void onDebuggerStateUpdated();
};

} // namespace Ui

#endif // SRC_UI_VARIABLES_H_
