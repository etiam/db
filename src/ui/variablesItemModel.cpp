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
   static const auto &vars = Core::state()->variables();
   static const auto &pvars = Core::state()->previousVariables();

   if (!index.isValid())
      return QVariant();

   if (index.column() == 1 && role == Qt::ForegroundRole)
   {
       // get variable's name (column 0)
       auto nameindex = index.sibling(index.row(), 0);
       auto name = data(nameindex).toString().toStdString();

       // if previous value exists and it's not current value return Qt::red
       const auto &variable = vars.at(name);
       if (pvars.find(name) != std::end(pvars))
       {
           const auto &previousvariable = pvars.at(name);

           if (boost::any_cast<std::string>(variable.value) != boost::any_cast<std::string>(previousvariable.value))
               return QColor(Qt::red);
       }
   }

   // Handle other roles

   return QStandardItemModel::data(index, role);
}

} // namespace Ui
