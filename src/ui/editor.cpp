/*
 * editor.cpp
 *
 *  Created on: Feb 6, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <iostream>

#include "core/signal.h"

#include "editorImpl.h"
#include "editor.h"

namespace Ui
{

Editor::Editor(QMainWindow *parent) :
    QWidget(parent),
    m_impl(std::make_unique<EditorImpl>(this))
{
    m_impl->startAceWidget();
    m_impl->executeJavaScript("editor.focus()");

    m_impl->executeJavaScript("editor.setShowPrintMargin(false)");
    m_impl->executeJavaScript("editor.setDisplayIndentGuides(false)");
    m_impl->executeJavaScript("editor.setReadOnly(true)");
    m_impl->executeJavaScript("editor.setShowFoldWidgets(false)");

    auto fontname = QFontDatabase::systemFont(QFontDatabase::FixedFont).family();
    m_impl->executeJavaScript(QString("editor.setOptions({ fontFamily: \"%1\" })").arg(fontname));

    setHighlightMode("c_cpp");
    setTheme("clouds_midnight");

    Core::loadFileSignal.connect(this, &Editor::onLoadFileSignal);
    Core::setBreakpointSignal.connect(this, &Editor::onSetBreakpointSignal);
    Core::setCursorPositionSignal.connect(this, &Editor::onSetCursorPositionSignal);
}

Editor::~Editor()
{
}

void
Editor::setGutterWidth(int width)
{
    if (width > 0)
    {
        m_impl->executeJavaScript(QString("editor.session.gutterRenderer.setWidth(%1)").arg(width));
    }
}

void
Editor::setText(const QString &newText)
{
    const QString request = "editor.getSession().setValue('%1')";
    m_impl->executeJavaScript(request.arg(m_impl->escape(newText)));

    setCursorPosition(1, 1);
}

QString
Editor::getText()
{
    return m_impl->executeJavaScript(QString("editor.getSession().getValue()")).toString();
}

void
Editor::setTheme(const QString &name)
{
    const QString request = ""
            "$.getScript('%1');"
            "editor.setTheme('ace/theme/%2');";
    m_impl->executeJavaScript(request.arg("qrc:/ace/theme-"+name+".js").arg(name));
}

void
Editor::setHighlightMode(const QString &name)
{
    const QString request = ""
            "$.getScript('%1');"
            "editor.getSession().setMode('ace/mode/%2');";
    m_impl->executeJavaScript(request.arg("qrc:/ace/mode-"+name+".js").arg(name));
}

void
Editor::setKeyboardHandler(const QString &name)
{
    const QString request = ""
            "$.getScript('%1');"
            "editor.getSession().setMode('ace/keybinding/%2');";
    m_impl->executeJavaScript(request.arg("qrc:/ace/keybinding-"+name+".js").arg(name));
}

QString
Editor::getLineText(int row) const
{
    return m_impl->executeJavaScript(QString("editor.getSession().getLine(%1)").arg(row)).toString();
}

int
Editor::getNumLines() const
{
    return m_impl->executeJavaScript("property('lines')").toInt();
}

int
Editor::getLineLength(int row) const
{
    return getLineText(row).length();
}

// wink signal handlers

void
Editor::onLoadFileSignal(const std::string &filename)
{
    QMetaObject::invokeMethod(this, "loadFile", Qt::QueuedConnection, Q_ARG(QString, QString::fromStdString(filename)));
}

void
Editor::onSetBreakpointSignal(int row)
{
    QMetaObject::invokeMethod(this, "setBreakpoint", Qt::QueuedConnection, Q_ARG(int, row));
}

void
Editor::onSetCursorPositionSignal(int row, int column)
{
    QMetaObject::invokeMethod(this, "setCursorPosition", Qt::QueuedConnection, Q_ARG(int, row), Q_ARG(int, column));
}

// qt slots

void
Editor::loadFile(const QString &filename)
{
    // read file into editor
    if (!filename.isEmpty())
    {
        QFile file(filename);
        QTextStream stream(&file);

        file.open(QFile::ReadOnly | QFile::Text);
        auto text = stream.readAll();
        setText(text);

        auto numlines = text.count("\n");
        auto numdigits = numlines > 0 ? (int) log10((double) numlines) + 1 : 1;
        setGutterWidth(numdigits);
    }
}

void
Editor::setBreakpoint(int row)
{
    m_impl->executeJavaScript(QString("editor.getSession().setBreakpoint(%1)").arg(row-1));
}

void
Editor::setCursorPosition(int row, int column)
{
    if (getNumLines() > row && getLineLength(row) >= column)
    {
        m_impl->executeJavaScript(QString("editor.moveCursorTo(%1, %2)").arg(row-1).arg(column-1));
        m_impl->executeJavaScript(QString("editor.renderer.scrollCursorIntoView(null, 0.5)"));
    }
}

}
