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

#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QtWebKit>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWebKitWidgets>
#endif

#include "editorImpl.h"
#include "editor.h"

Editor::Editor(QWidget *parent) :
    QWidget(parent),
    d(new EditorImpl(this))
{
    d->startAceWidget();
    d->executeJavaScript("editor.focus()");

    d->executeJavaScript("editor.setFontSize(16)");
    d->executeJavaScript("editor.setCursorStyle(\"wide\")");
    d->executeJavaScript("editor.setKeyboardHandler(vim)");
    d->executeJavaScript("editor.setShowPrintMargin(false)");
    d->executeJavaScript("editor.setDisplayIndentGuides(false)");
    d->executeJavaScript("editor.setReadOnly(true)");
}

QString
Editor::line(int row) const
{
    return d->executeJavaScript(QString("editor.getSession().getLine(%1)").arg(row)).toString();
}

int
Editor::lines() const
{
    return d->executeJavaScript("property('lines')").toInt();
}

int Editor::lineLength(int row) const
{
    return line(row).length();
}

void
Editor::setCursorPosition(int row, int column)
{
    if (lines() > row && lineLength(row) >= column)
    {
        d->executeJavaScript(QString("editor.moveCursorTo(%1, %2)").arg(row).arg(column));
    }
}
void
Editor::setText(const QString &newText)
{
    const QString request = "editor.getSession().setValue('%1')";
    std::cout << newText.toStdString();
    d->executeJavaScript(request.arg(d->escape(newText)));
    int lastLine = lines()-1;
    setCursorPosition(lastLine, lineLength(lastLine));
}

void
Editor::setTheme(const QString &name, const QUrl &url)
{
    const QString request = ""
            "$.getScript('%1');"
            "editor.setTheme('ace/theme/%2');";
    d->executeJavaScript(request.arg(url.toString()).arg(name));
}

void
Editor::setTheme(const QString &name)
{
    const QString request = ""
            "$.getScript('%1');"
            "editor.setTheme('ace/theme/%2');";
    d->executeJavaScript(request.arg("qrc:/ace/theme-"+name+".js").arg(name));
}

void
Editor::setHighlightMode(const QString &name, const QUrl &url)
{
    const QString request = ""
            "$.getScript('%1');"
            "editor.getSession().setMode('ace/mode/%2');";
    d->executeJavaScript(request.arg(url.toString()).arg(name));
}

void
Editor::setHighlightMode(const QString &name)
{
    const QString request = ""
            "$.getScript('%1');"
            "editor.getSession().setMode('ace/mode/%2');";
    d->executeJavaScript(request.arg("qrc:/ace/mode-"+name+".js").arg(name));
}

