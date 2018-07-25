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
#include <QFileInfo>

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
    m_model->setHeaderData(4, Qt::Horizontal, tr("HitCount"));

    setRootIsDecorated(false);
    setIndentation(10);

    setModel(m_model);

    // column width defaults
    setColumnWidth(0, 30);
    setColumnWidth(1, 30);
    setColumnWidth(2, 150);
    setColumnWidth(3, 300);
    setColumnWidth(4, 30);

    // prevent resize of 1st column width
    header()->setSectionResizeMode(0, QHeaderView::Fixed);
    header()->setSectionResizeMode(1, QHeaderView::Fixed);
    header()->setSectionResizeMode(4, QHeaderView::Fixed);

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
    for (const auto &breakpoint : Core::state()->breakPoints().getAll())
    {
        auto rowcount = m_model->rowCount();
        m_model->insertRow(rowcount);

        QFileInfo fileinfo(QString::fromStdString(breakpoint.location.filename));
        QString filename = fileinfo.fileName() + ", line " + QString::number(breakpoint.location.row);

        m_model->setData(m_model->index(rowcount, 0), "");
        m_model->setData(m_model->index(rowcount, 1), breakpoint.breakpointnumber);
        m_model->setData(m_model->index(rowcount, 2), QString::fromStdString(breakpoint.location.function));
        m_model->setData(m_model->index(rowcount, 3), filename);
        m_model->setData(m_model->index(rowcount, 4), breakpoint.hitcount);

        if (breakpoint.enabled)
            m_model->setData(m_model->index(rowcount, 0), QIcon(":/img/brkp"), Qt::DecorationRole);
        else
            m_model->setData(m_model->index(rowcount, 0), QIcon(":/img/brkp_disabled"), Qt::DecorationRole);
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

void
BreakPoints::mousePressEvent(QMouseEvent *event)
{
    const auto index = indexAt(event->pos());
    selectionModel()->select(index.sibling(index.row(), 3), QItemSelectionModel::ClearAndSelect);
}

} // namespace Ui
