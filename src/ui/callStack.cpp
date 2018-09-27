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

#include "callStackItemModel.h"
#include "callStack.h"

namespace Ui
{

CallStack::CallStack(QWidget *parent) :
    QTreeView(parent)
{
    setObjectName("callstack");
    setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    m_model = new CallStackItemModel(0, 3, this);

    m_model->setHeaderData(0, Qt::Horizontal, "#");
    m_model->setHeaderData(1, Qt::Horizontal, tr("Function"));
    m_model->setHeaderData(2, Qt::Horizontal, tr("Filename"));

    setRootIsDecorated(false);
    setIndentation(10);

    setModel(m_model);

    // column width defaults
    setColumnWidth(0, 20);
    setColumnWidth(1, 150);
    setColumnWidth(2, 300);

    // prevent resize of 1st column width
    header()->setSectionResizeMode(0, QHeaderView::Fixed);
    header()->setSectionResizeMode(1, QHeaderView::Fixed);

    // don't allow columns to be re-ordered
    header()->setSectionsMovable(false);

    // needed for int types in QStandardItemModel
    qRegisterMetaType<QVector<int>>("QVector<int>");

    Core::Signals::callStackUpdated.connect(this, &CallStack::onCallStackUpdated);
}

void
CallStack::onCallStackUpdated()
{
    // clear all rows from model
    m_model->removeRows(0, m_model->rowCount());

    // populate model from call stack data
    for (const auto &stackline : Core::state()->callStack())
    {
        auto rowcount = m_model->rowCount();
        m_model->insertRow(rowcount);

        QFileInfo fileinfo(QString::fromStdString(stackline.location.filename));
        QString filename = fileinfo.fileName() + ", line " + QString::number(stackline.location.row);

        m_model->setData(m_model->index(rowcount, 0), stackline.level);
        m_model->setData(m_model->index(rowcount, 1), QString::fromStdString(stackline.location.function));
        m_model->setData(m_model->index(rowcount, 2), filename);
    }

    setCurrentIndex(indexAt(QPoint(0, 0)));
}

void
CallStack::mouseDoubleClickEvent(QMouseEvent *event)
{
    // do nothing
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
        case Qt::Key_Up:
        case Qt::Key_Down:
            {
            QTreeView::keyPressEvent(event);

            const auto row = currentIndex().row();
            loadSourceAtRow(row);
            break;
            }

        default:
            QTreeView::keyPressEvent(event);
    }
}

void
CallStack::loadSourceAtRow(int row)
{
    const auto &stack = Core::state()->callStack();

    // bounds check
    if (row <= stack.size())
    {
        const auto location = stack[row].location;

        Core::Signals::loadEditorSource.emit(location.filename);
        Core::Signals::setCursorLocation.emit(location);
    }
}

} // namespace Ui
