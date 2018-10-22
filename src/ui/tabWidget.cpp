/*
 * tabWidget.cpp
 *
 *  Created on: Oct 21, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <iostream>
#include "tabWidget.h"

namespace Ui
{

TabWidget::TabWidget(QWidget *parent) :
    QTabWidget(parent)
{
}

void
TabWidget::enterEvent(QEvent *event)
{
    // TODO : draw border around tab widget

    QTabWidget::enterEvent(event);
}

} // namespace Ui
