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

private:
    int m_currentFrame = 0;
    VariablesItemModel * m_model;

private Q_SLOTS:
    void onCallStackUpdated();
    void onVariablesUpdated();
    void onDebuggerStateUpdated();
};

} // namespace Ui

#endif // SRC_UI_VARIABLES_H_
