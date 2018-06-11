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

/*
QVariant
CallStackItemModel::data(const QModelIndex& index, int role) const
{
    QVariant result = QVariant();

    std::cout << index.row() << " " << index.column() << std::endl;
//
//    if (index.isValid())
//    {
//        if (role == Qt::DecorationRole && index.column() == 0)
//            result = QPixmap(":/img/paused");
//
//        else
            result = QStandardItemModel::data(index, role);
//    }

//    if (index.isValid())
//    {
//        switch (role)
//        {
//            case Qt::DecorationRole:
//                result = QPixmap(":/img/paused");
//                break;
//
//            default:
//                result = QStandardItemModel::data(index, role);
//                break;
//        }
//    }

    return result;
}
*/

} // namespace Ui
