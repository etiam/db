/*
 * breakPoint.cpp
 *
 *  Created on: Jun 6, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <iostream>

#include <QHeaderView>
#include <QStandardItemModel>
#include <QFontDatabase>
#include <QMouseEvent>

#include "core/signal.h"
#include "core/global.h"
#include "core/state.h"

#include "breakPointsItemModel.h"
#include "breakPoints.h"

namespace Ui
{

BreakPoints::BreakPoints(QWidget *parent) :
    QTreeView(parent)
{
    setObjectName("breakpoint");
    setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    m_model = new BreakPointsItemModel(0, 5, this);

    m_model->setHeaderData(0, Qt::Horizontal, "", Qt::DisplayRole);
    m_model->setHeaderData(1, Qt::Horizontal, "#");
    m_model->setHeaderData(2, Qt::Horizontal, tr("Function"));
    m_model->setHeaderData(3, Qt::Horizontal, tr("Filename"));
    m_model->setHeaderData(4, Qt::Horizontal, tr("Line"));

    setRootIsDecorated(false);
    setIndentation(10);

    setModel(m_model);

    // column width defaults
    setColumnWidth(0, 30);
    setColumnWidth(1, 20);
    setColumnWidth(2, 150);
    setColumnWidth(3, 300);

    // prevent resize of 1st column width
    header()->setSectionResizeMode(0, QHeaderView::Fixed);
    header()->setSectionResizeMode(1, QHeaderView::Fixed);

    // don't allow columns to be re-ordered
    header()->setSectionsMovable(false);

    // needed for int types in QStandardItemModel
    qRegisterMetaType<QVector<int>>("QVector<int>");

    Core::Signal::breakPointsUpdated.connect(this, &BreakPoints::onBreakPointsUpdated);
}

void
BreakPoints::onBreakPointsUpdated()
{
    // clear all rows from model
    m_model->removeRows(0, m_model->rowCount());

    // populate model from call stack data
    for (const auto &entry : Core::state()->breakPoints().getAll())
    {
        auto rowcount = m_model->rowCount();
        m_model->insertRow(rowcount);

        m_model->setData(m_model->index(rowcount, 0), "");
        m_model->setData(m_model->index(rowcount, 1), entry.breakpointnumber);
        m_model->setData(m_model->index(rowcount, 2), entry.breakpointnumber);
        m_model->setData(m_model->index(rowcount, 3), QString::fromStdString(entry.location.filename));
        m_model->setData(m_model->index(rowcount, 4), entry.location.row);
    }
}

void
BreakPoints::mouseDoubleClickEvent(QMouseEvent *event)
{
//    const auto row = indexAt(event->pos()).row();
//    const auto &stack = Core::state()->breakPoints();
//    if (row <= stack.size())
//    {
//        const auto callback = stack[row];
//
//        std::cout << callback.location.filename << std::endl;
//
//        // load editor with contents of filename
//        Core::Signal::loadFile(callback.location.filename);
//
//        // update global current location
//        Core::Signal::setCurrentLocation(callback.location);
//    }
}

} // namespace Ui
