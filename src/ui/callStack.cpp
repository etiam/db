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
#include <QFileInfo>

#include "core/signals.h"
#include "core/global.h"
#include "core/state.h"
#include "core/utils.h"

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
    m_model->setHeaderData(1, Qt::Horizontal, "#");
    m_model->setHeaderData(2, Qt::Horizontal, tr("Function"));
    m_model->setHeaderData(3, Qt::Horizontal, tr("Filename"));

    setRootIsDecorated(false);
    setIndentation(10);

    setModel(m_model);

    // column width defaults
    setColumnWidth(0, 30);
    setColumnWidth(1, 40);
    setColumnWidth(2, 150);
    setColumnWidth(3, 300);

    // prevent resize of 1st column width
    header()->setSectionResizeMode(0, QHeaderView::Fixed);
    header()->setSectionResizeMode(1, QHeaderView::Fixed);

    // don't allow columns to be re-ordered
    header()->setSectionsMovable(false);

    // needed for int types in QStandardItemModel
    qRegisterMetaType<QVector<int>>("QVector<int>");

    // connect signal handlers
    Core::Signals::callStackUpdated.connect(this, &CallStack::onCallStackUpdated);
    Core::Signals::debuggerStateUpdated.connect(this, &CallStack::onDebuggerStateUpdated);
}

void
CallStack::onCallStackUpdated()
{
    // clear all rows from model
    m_model->removeRows(0, m_model->rowCount());

    auto &callstack = Core::state()->callStack();
    auto currentframe = callstack.currentFrame();

    // populate model from call stack data
    for (auto &&entry : Core::enumerate(callstack.entries()))
    {
        auto index = std::get<0>(entry);
        auto stackline = std::get<1>(entry);

        auto rowcount = m_model->rowCount();
        m_model->insertRow(rowcount);

        QFileInfo fileinfo(QString::fromStdString(stackline.location.filename));
        QString filename = fileinfo.fileName() + ", line " + QString::number(stackline.location.row);

        // draw icon indicating breakpoint status in first column
        if (index == currentframe)
            m_model->setData(m_model->index(rowcount, 0), QIcon(":/img/currentline"), Qt::DecorationRole);

        m_model->setData(m_model->index(rowcount, 1), stackline.level);
        m_model->setData(m_model->index(rowcount, 2), QString::fromStdString(stackline.location.function));
        m_model->setData(m_model->index(rowcount, 3), filename);
    }

    setCurrentIndex(model()->index(currentframe, 0));
}

void
CallStack::mouseDoubleClickEvent(QMouseEvent *event)
{
    const auto row = indexAt(event->pos()).row();

    Core::state()->callStack().setCurrentFrame(row);
    Core::Signals::callStackUpdated.emit();
}

void
CallStack::mousePressEvent(QMouseEvent *event)
{
    const auto row = indexAt(event->pos()).row();
    loadSourceAtRow(row);

    QTreeView::mousePressEvent(event);
}

void
CallStack::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
        // move up or down then load the stack frame of the moved to row
        case Qt::Key_Up:
        case Qt::Key_Down:
            {
            QTreeView::keyPressEvent(event);

            const auto row = currentIndex().row();
            loadSourceAtRow(row);
            break;
            }

        // make the stack frame at the current row the current frame
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Space:
        {
            const auto row = currentIndex().row();
            Core::state()->callStack().setCurrentFrame(row);
            Core::Signals::callStackUpdated.emit();
            break;
        }

        default:
            QTreeView::keyPressEvent(event);
    }
}

void
CallStack::loadSourceAtRow(int row)
{
    const auto &stack = Core::state()->callStack().entries();

    // bounds check
    if (row <= stack.size())
    {
        const auto location = stack[row].location;

        Core::Signals::loadEditorSource.emit(location.filename);
        Core::Signals::highlightLocation.emit(location);
        Core::Signals::setCursorLocation.emit(location);
    }
}


void
CallStack::onDebuggerStateUpdated()
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
