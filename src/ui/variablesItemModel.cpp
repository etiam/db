/*
 * variables.cpp
 *
 *  Created on: Oct 22, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "variablesItemModel.h"

namespace Ui
{

VariablesItemModel::VariablesItemModel(QObject *parent) :
    QStandardItemModel(parent)
{
}

VariablesItemModel::VariablesItemModel(int rows, int columns, QObject* parent) :
    QStandardItemModel(rows, columns, parent)
{
}

} // namespace Ui
