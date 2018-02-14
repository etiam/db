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

Editor::Editor(QMainWindow *parent) :
    QWidget(parent),
    d(new EditorImpl(this)),
    m_parent(parent)
{
    d->startAceWidget();
    d->executeJavaScript("editor.focus()");

    d->executeJavaScript("editor.setFontSize(16)");
    d->executeJavaScript("editor.setKeyboardHandler(\"ace/keyboard/vim\")");
    d->executeJavaScript("editor.setShowPrintMargin(false)");
    d->executeJavaScript("editor.setDisplayIndentGuides(false)");
    d->executeJavaScript("editor.setReadOnly(true)");
    d->executeJavaScript("editor.setShowFoldWidgets(false)");

    setHighlightMode("c_cpp");
    setTheme("clouds_midnight");
}

void
Editor::setGutterWidth(int width)
{
    if (width > 0)
    {
        d->executeJavaScript(QString("editor.session.gutterRenderer.setWidth(%1)").arg(width));
    }
}

void
Editor::setCursorPosition(int row, int column)
{
    if (getNumLines() > row && getLineLength(row) >= column)
    {
        d->executeJavaScript(QString("editor.moveCursorTo(%1, %2)").arg(row).arg(column));
        d->executeJavaScript("editor.renderer.scrollCursorIntoView()");
    }
}

void
Editor::setText(const QString &newText)
{
    const QString request = "editor.getSession().setValue('%1')";
    d->executeJavaScript(request.arg(d->escape(newText)));
    QPoint p(0, 0);
    int row = 0;
    for (const QString &line : newText.split("\n"))
    {
        if (line.length() > p.y())
        {
            p = QPoint(row, line.length());
        }
        ++row;
    }

    std::cout << p.x() << " " << p.y() << std::endl;
    setCursorPosition(p.x(), p.y());

    setCursorPosition(0, 0);
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

void
Editor::setKeyboardHandler(const QString &name)
{
    const QString request = ""
            "$.getScript('%1');"
            "editor.getSession().setMode('ace/keybinding/%2');";
    d->executeJavaScript(request.arg("qrc:/ace/keybinding-"+name+".js").arg(name));
}

QString
Editor::getLineText(int row) const
{
    return d->executeJavaScript(QString("editor.getSession().getLine(%1)").arg(row)).toString();
}

int
Editor::getNumLines() const
{
    return d->executeJavaScript("property('lines')").toInt();
}

int
Editor::getLineLength(int row) const
{
    return getLineText(row).length();
}

bool
Editor::eventFilter(QObject *object, QEvent *filteredEvent)
{
    // Key press filters
    if (filteredEvent->type() == QEvent::KeyPress)
    {
        QKeyEvent *event = (QKeyEvent*) filteredEvent;
        int key = event->key();

        if (key == Qt::Key_Q)
        {
            QCoreApplication::quit();
            return true;
        }
    }

    return false;
}
