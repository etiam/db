/*
 * variables.cpp
 *
 *  Created on: Oct 22, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "core/global.h"
#include "core/state.h"

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

QVariant
VariablesItemModel::data(const QModelIndex &index, int role) const
{
   static const auto &state = Core::state();

   if (!index.isValid())
      return QVariant();

   if (index.column() == 1 && role == Qt::ForegroundRole)
   {
       if (state->previousVariables().size() > 0)
       {
           const auto &variable = state->variables()[index.row()];
           const auto &previousvariable = state->previousVariables()[index.row()];

           if (boost::any_cast<std::string>(variable.value) != boost::any_cast<std::string>(previousvariable.value))
              return QColor(Qt::red);
       }
   }

   // Handle other roles

   return QStandardItemModel::data(index, role);
}

} // namespace Ui
