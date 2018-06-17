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
    setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    m_model = new CallStackItemModel(0, 4, this);

    m_model->setHeaderData(0, Qt::Horizontal, "", Qt::DisplayRole);
    m_model->setHeaderData(1, Qt::Horizontal, QObject::tr("Function"));
    m_model->setHeaderData(2, Qt::Horizontal, QObject::tr("Filename"));
    m_model->setHeaderData(3, Qt::Horizontal, QObject::tr("Line"));

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
    m_model->removeRows(0, m_model->rowCount());

    const auto currentlocation = Core::state()->currentLocation();

    for (const auto &entry : Core::state()->callStack())
    {
        auto rowcount = m_model->rowCount();
        m_model->insertRow(rowcount);

        m_model->setData(m_model->index(rowcount, 0), "");
        m_model->setData(m_model->index(rowcount, 1), QString::fromStdString(entry.function));
        m_model->setData(m_model->index(rowcount, 2), QString::fromStdString(entry.filename));
        m_model->setData(m_model->index(rowcount, 3), entry.location.row);

        if (currentlocation == entry.location)
            m_model->setData(m_model->index(rowcount, 0), QIcon(":/img/currentline"), Qt::DecorationRole);
    }
}

void
CallStack::mouseDoubleClickEvent(QMouseEvent *event)
{
    const auto row = indexAt(event->pos()).row();
    const auto &stack = Core::state()->callStack();
    if (row <= stack.size())
    {
        const auto callback = stack[row];

        std::cout << callback.location.filename << std::endl;

        // load editor with contents of filename
        Core::Signal::loadFile(callback.location.filename);

        // update global current location
        Core::Signal::setCurrentLocation(callback.location);
    }
}

} // namespace Ui
