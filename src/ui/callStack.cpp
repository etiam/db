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

#include <QStandardItemModel>

#include "core/signal.h"
#include "core/global.h"
#include "core/state.h"

#include "callStack.h"

namespace Ui
{

CallStack::CallStack(QWidget *parent) :
    QTreeView(parent)
{
    m_model = new QStandardItemModel(0, 4, this);

    m_model->setHeaderData(0, Qt::Horizontal, QObject::tr("Level"));
    m_model->setHeaderData(1, Qt::Horizontal, QObject::tr("Function"));
    m_model->setHeaderData(2, Qt::Horizontal, QObject::tr("Filename"));
    m_model->setHeaderData(3, Qt::Horizontal, QObject::tr("Line"));

    setModel(m_model);

    // needed for int types in QStandardItemModel
    qRegisterMetaType<QVector<int>>("QVector<int>");

    Core::Signal::callStackUpdated.connect(this, &CallStack::onCallStackUpdated);
}

void
CallStack::onCallStackUpdated()
{
    m_model->removeRows(0, m_model->rowCount());

    for (const auto &entry : Core::state()->callStack())
    {
        auto rowcount = m_model->rowCount();
        m_model->insertRow(rowcount);

        m_model->setData(m_model->index(rowcount, 0), entry.level);
        m_model->setData(m_model->index(rowcount, 1), QString::fromStdString(entry.function));
        m_model->setData(m_model->index(rowcount, 2), QString::fromStdString(entry.filename));
        m_model->setData(m_model->index(rowcount, 3), entry.line);
    }
}

} // namespace Ui
