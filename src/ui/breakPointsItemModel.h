/*
 * breakPointsItemModel.h
 *
 *  Created on: Jul 22, 2018
 *      Author: jasonr
 */

#ifndef UI_BREAKPOINTSITEMMODEL_H_
#define UI_BREAKPOINTSITEMMODEL_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <QStandardItemModel>

namespace Ui
{

class BreakPointsItemModel : public QStandardItemModel
{
  Q_OBJECT

  public:
    explicit BreakPointsItemModel(QObject *parent = nullptr);
    BreakPointsItemModel(int rows, int columns, QObject *parent = nullptr);

    ~BreakPointsItemModel() = default;
};

} // namespace Ui

#endif // UI_BREAKPOINTSITEMMODEL_H_
