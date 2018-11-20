/*
 * variables.cpp
 *
 *  Created on: Oct 22, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <QHeaderView>
#include <QFontDatabase>

#include "core/signals.h"
#include "core/global.h"

#include "gdb/global.h"
#include "gdb/commands.h"

#include "variablesItemModel.h"
#include "variables.h"

namespace Ui
{

Variables::Variables(QWidget *parent) :
    QTreeView(parent)
{
    setObjectName("callstack");
    setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    m_model = new VariablesItemModel(0, 3, this);

    m_model->setHeaderData(0, Qt::Horizontal, tr("Name"));
    m_model->setHeaderData(1, Qt::Horizontal, tr("Value"));
    m_model->setHeaderData(2, Qt::Horizontal, tr("Type"));

    setRootIsDecorated(false);
    setIndentation(10);

    setModel(m_model);

    // column width defaults
    setColumnWidth(0, 150);
    setColumnWidth(1, 150);
    setColumnWidth(2, 150);

    // allow column sorting
    setSortingEnabled(true);

    // don't allow columns to be re-ordered
    header()->setSectionsMovable(false);

    // show sort indicator
    header()->setSortIndicatorShown(true);

    // needed for int types in QStandardItemModel
    qRegisterMetaType<QVector<int>>("QVector<int>");

    // connect signal handlers
    Core::Signals::callStackUpdated.connect([this]()
    {
        QMetaObject::invokeMethod(this, "onCallStackUpdated", Qt::QueuedConnection);
    });

    Core::Signals::variablesUpdated.connect([this]()
    {
        QMetaObject::invokeMethod(this, "onVariablesUpdated", Qt::QueuedConnection);
    });

    Core::Signals::debuggerStateUpdated.connect([this]()
    {
        QMetaObject::invokeMethod(this, "onDebuggerStateUpdated", Qt::QueuedConnection);
    });
}

void
Variables::setTabFocus()
{
    setFocus();
}

void
Variables::onCallStackUpdated()
{
    auto currentframe = Core::state()->currentStackFrame();

    if (currentframe != m_currentFrame)
    {
        m_currentFrame = currentframe;

        // trigger update of variables from gdb
        Gdb::commands()->updateVariables();
    }
}

void
Variables::onVariablesUpdated()
{
    // clear all rows from model
    m_model->removeRows(0, m_model->rowCount());

    auto &variables = Core::state()->variables();

    // populate model from call stack data
    for (const auto &variable : variables)
    {
        auto rowcount = m_model->rowCount();
        m_model->insertRow(rowcount);

        m_model->setData(m_model->index(rowcount, 0), QString::fromStdString(variable.first));
        m_model->setData(m_model->index(rowcount, 1), QString::fromStdString(boost::any_cast<std::string>(variable.second.value)));
        m_model->setData(m_model->index(rowcount, 2), QString::fromStdString(variable.second.type));
    }
}

void
Variables::onDebuggerStateUpdated()
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
