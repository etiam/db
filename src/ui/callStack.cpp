/*
 * callStack.cpp
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

#include "callStackItemModel.h"
#include "callStack.h"

namespace Ui
{

CallStack::CallStack(QWidget *parent) :
    QTreeView(parent)
{
    setObjectName("callstack");
    setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    m_model = new CallStackItemModel(0, 4, this);

    m_model->setHeaderData(0, Qt::Horizontal, "", Qt::DisplayRole);
    m_model->setHeaderData(1, Qt::Horizontal, tr("Function"));
    m_model->setHeaderData(2, Qt::Horizontal, tr("Filename"));
    m_model->setHeaderData(3, Qt::Horizontal, tr("Line"));

    setRootIsDecorated(false);
    setIndentation(10);

    setModel(m_model);

    // column width defaults
    setColumnWidth(0, 30);
    setColumnWidth(1, 150);
    setColumnWidth(2, 300);

    // prevent resize of 1st column width
    header()->setSectionResizeMode(0, QHeaderView::Fixed);

    // don't allow columns to be re-ordered
    header()->setSectionsMovable(false);

    // needed for int types in QStandardItemModel
    qRegisterMetaType<QVector<int>>("QVector<int>");

    Core::Signal::callStackUpdated.connect(this, &CallStack::onCallStackUpdated);
}

void
CallStack::onCallStackUpdated()
{
    // clear all rows from model
    m_model->removeRows(0, m_model->rowCount());

    // populate model from call stack data
    for (const auto &entry : Core::state()->callStack())
    {
        auto rowcount = m_model->rowCount();
        m_model->insertRow(rowcount);

        m_model->setData(m_model->index(rowcount, 0), "");
        m_model->setData(m_model->index(rowcount, 1), QString::fromStdString(entry.location.function));
        m_model->setData(m_model->index(rowcount, 2), QString::fromStdString(entry.location.filename));
        m_model->setData(m_model->index(rowcount, 3), entry.location.row);

        // draw indicator in first column if this entry is the current frame
        const auto currentlocation = Core::state()->currentLocation();
        if (currentlocation == entry.location)
            m_model->setData(m_model->index(rowcount, 0), QIcon(":/img/currentline"), Qt::DecorationRole);
    }
}

void
CallStack::mouseDoubleClickEvent(QMouseEvent *event)
{
    const auto row = indexAt(event->pos()).row();
    const auto &stack = Core::state()->callStack();

    // bounds check
    if (row <= stack.size())
    {
        const auto location = stack[row].location;

        // load editor with contents of filename
        Core::Signal::loadFile(location.filename);

        // update current location
        Core::Signal::setCurrentLocation(location);
    }
}

} // namespace Ui
