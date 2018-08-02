#
# Copyright (C) 2012-2015 Mattia Basaglia
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

CONFIG += plugin
QT += designer widgets
TARGET = history_lineedit
TEMPLATE = lib
OBJECTS_DIR = ../out/obj
MOC_DIR = ../out/generated
UI_DIR = ../out/generated
RCC_DIR = ../out/generated

SOURCES += \
    history_lineedit_plugin.cpp \
    history_lineedit_plugin_collection.cpp

HEADERS += \
    history_lineedit_plugin.hpp \
    history_lineedit_plugin_collection.hpp

include(../history_lineedit.pri)

build_all:!build_pass {
 CONFIG -= build_all
 CONFIG += release
}

# install
target.path += $$[QT_INSTALL_PLUGINS]/designer

unix{
    LIB_TARGET = lib$${TARGET}.so
}
win32 {
    LIB_TARGET = $${TARGET}.dll
}
creator.files =$$LIB_TARGET
creator.path = $$[QT_INSTALL_PREFIX]/../../Tools/QtCreator/bin/designer
INSTALLS += target creator

