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

    // don't allow columns to be re-ordered
    header()->setSectionsMovable(false);

    // needed for int types in QStandardItemModel
    qRegisterMetaType<QVector<int>>("QVector<int>");

    // connect signal handlers
//    Core::Signals::callStackUpdated.connect([this]()
//    {
//        QMetaObject::invokeMethod(this, "onCallStackUpdated", Qt::QueuedConnection);
//    });

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
Variables::mouseDoubleClickEvent(QMouseEvent *event)
{
//    const auto row = indexAt(event->pos()).row();
//
//    Core::state()->callStack().setCurrentFrame(row);
//    Core::Signals::callStackUpdated.emit();
}

void
Variables::mousePressEvent(QMouseEvent *event)
{
//    const auto row = indexAt(event->pos()).row();
//    loadSourceAtRow(row);

    QTreeView::mousePressEvent(event);
}

void
Variables::keyPressEvent(QKeyEvent *event)
{
//    switch (event->key())
//    {
//        // move up or down then load the stack frame of the moved to row
//        case Qt::Key_Up:
//        case Qt::Key_Down:
//            {
//            QTreeView::keyPressEvent(event);
//
//            const auto row = currentIndex().row();
//            loadSourceAtRow(row);
//            break;
//            }
//
//        // make the stack frame at the current row the current frame
//        case Qt::Key_Enter:
//        case Qt::Key_Return:
//        case Qt::Key_Space:
//        {
//            const auto row = currentIndex().row();
//            Core::state()->callStack().setCurrentFrame(row);
//            Core::Signals::callStackUpdated.emit();
//            break;
//        }
//
//        default:
//            QTreeView::keyPressEvent(event);
//    }
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
