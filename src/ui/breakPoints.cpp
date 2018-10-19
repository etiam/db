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

#include "core/signals.h"
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
    setColumnWidth(1, 40);
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

    // connect signal handlers
    Core::Signals::breakPointsUpdated.connect(this, &BreakPoints::onBreakPointsUpdated);
    Core::Signals::debuggerStateUpdated.connect(this, &BreakPoints::onDebuggerStateUpdated);
}

void
BreakPoints::mouseDoubleClickEvent(QMouseEvent *event)
{
}

void
BreakPoints::mousePressEvent(QMouseEvent *event)
{
    const auto index = indexAt(event->pos());
    selectionModel()->select(index.sibling(index.row(), 3), QItemSelectionModel::ClearAndSelect);
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

        // draw icon indicating breakpoint status in first column
        if (breakpoint.enabled)
            m_model->setData(m_model->index(rowcount, 0), QIcon(":/img/brkp"), Qt::DecorationRole);
        else
            m_model->setData(m_model->index(rowcount, 0), QIcon(":/img/brkp_disabled"), Qt::DecorationRole);

        m_model->setData(m_model->index(rowcount, 1), breakpoint.breakpointnumber);
        m_model->setData(m_model->index(rowcount, 2), QString::fromStdString(breakpoint.location.function));
        m_model->setData(m_model->index(rowcount, 3), filename);
        m_model->setData(m_model->index(rowcount, 4), breakpoint.hitcount);
    }
}

void
BreakPoints::onDebuggerStateUpdated()
{
    const auto state = Core::state()->debuggerState();

    switch(state)
    {
        case Core::State::Debugger::PAUSED:
        case Core::State::Debugger::LOADED:
            setDisabled(false);
            break;

        case Core::State::Debugger::RUNNING:
            setDisabled(true);
            break;

        default:
            break;
    }
}

} // namespace Ui
