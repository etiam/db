/*
 * callStackItemModel.cpp
 *
 *  Created on: Jun 9, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <iostream>

#include "callStackItemModel.h"

namespace Ui
{

CallStackItemModel::CallStackItemModel(QObject *parent) :
    QStandardItemModel(parent)
{
}

CallStackItemModel::CallStackItemModel(int rows, int columns, QObject* parent) :
    QStandardItemModel(rows, columns, parent)
{
}

} // namespace Ui
