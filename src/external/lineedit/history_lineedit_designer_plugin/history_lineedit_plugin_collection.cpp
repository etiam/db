/**
 * \file
 *
 * \author Mattia Basaglia
 *
 * \copyright Copyright (C) 2012-2015 Mattia Basaglia
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "history_lineedit_plugin_collection.hpp"
#include "history_lineedit_plugin.hpp"

History_LineEdit_Plugin_Collection::History_LineEdit_Plugin_Collection(QObject *parent) :
    QObject(parent)
{
    widgets.push_back(new History_LineEdit_Plugin(this));
}

QList<QDesignerCustomWidgetInterface *> History_LineEdit_Plugin_Collection::customWidgets() const
{
    return widgets;
}
