/*
 * breakPointsItemModel.cpp
 *
 *  Created on: Jul 22, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <iostream>

#include "breakPointsItemModel.h"

namespace Ui
{

BreakPointsItemModel::BreakPointsItemModel(QObject *parent) :
    QStandardItemModel(parent)
{
}

BreakPointsItemModel::BreakPointsItemModel(int rows, int columns, QObject* parent) :
    QStandardItemModel(rows, columns, parent)
{
}

} // namespace Ui
