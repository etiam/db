/*
 * tabWidget.h
 *
 *  Created on: Oct 21, 2018
 *      Author: jasonr
 */

#ifndef SRC_UI_TABWIDGET_H_
#define SRC_UI_TABWIDGET_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <QTabWidget>

namespace Ui
{

class TabWidget : public QTabWidget
{
public:
    TabWidget(QWidget *parent = nullptr);
    ~TabWidget() = default;

protected:
    void enterEvent(QEvent *event) override;
};

} // namespace Ui

#endif // SRC_UI_TABWIDGET_H_
