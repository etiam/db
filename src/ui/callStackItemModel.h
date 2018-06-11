/*
 * callStackItemModel.h
 *
 *  Created on: Jun 9, 2018
 *      Author: jasonr
 */

#ifndef UI_CALLSTACKITEMMODEL_H_
#define UI_CALLSTACKITEMMODEL_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <QStandardItemModel>

namespace Ui
{

class CallStackItemModel : public QStandardItemModel
{
  Q_OBJECT

  public:
    explicit CallStackItemModel(QObject *parent = nullptr);
    CallStackItemModel(int rows, int columns, QObject *parent = nullptr);

    ~CallStackItemModel() = default;
};

} // namespace Ui

#endif // UI_CALLSTACKITEMMODEL_H_
