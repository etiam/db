/*
 * variablesItemModel.h
 *
 *  Created on: Oct 22, 2018
 *      Author: jasonr
 */

#ifndef UI_VARIABLESITEMMODEL_H_
#define UI_VARIABLESITEMMODEL_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <QStandardItemModel>

namespace Ui
{

class VariablesItemModel : public QStandardItemModel
{
  Q_OBJECT

  public:
    explicit VariablesItemModel(QObject *parent = nullptr);
    VariablesItemModel(int rows, int columns, QObject *parent = nullptr);

    ~VariablesItemModel() = default;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
};

} // namespace Ui

#endif // UI_VARIABLESITEMMODEL_H_
