/*
 * mainWindow.cpp
 *
 *  Created on: Feb 6, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <iostream>
#include <sstream>

#include <QFile>
#include <QTextStream>
#include <QWidget>
#include <QVBoxLayout>
#include <QSettings>
#include <QMenu>
#include <QMenuBar>
#include <QSplitter>

#include "core/signal.h"

#include "editor.h"
#include "console.h"
#include "mainwindow.h"

Q_DECLARE_METATYPE(QList<int>)

namespace Ui
{

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    qRegisterMetaTypeStreamOperators<QList<int>>("QList<int>");

    setObjectName("mainwindow");
    setWindowTitle("db");

    // gui setup
    m_splitter = new QSplitter(this);
    m_splitter->setOrientation(Qt::Vertical);
    m_splitter->setObjectName("splitter");
    m_splitter->setContentsMargins(0, 0, 0, 0);
    setCentralWidget(m_splitter);

    // main editor window
    m_editor = new Editor();

    // tabs window
    m_tabWidget = new QTabWidget();
    m_tabWidget->setTabsClosable(true);

    // tabs within tab window
    m_console = new Console();
    m_tabWidget->addTab(m_console, tr("Console"));

    // add editor and tabwidget to main
    m_splitter->addWidget(m_editor);
    m_splitter->addWidget(m_tabWidget);

    // settings
    readSettings();

    createMenus();
    createHotkeys();

    Core::loadFileSignal.connect(this, &MainWindow::onLoadFileSignal);
    Core::loadFileCompleteSignal.connect(this, &MainWindow::onLoadFileCompleteSignal);
    Core::setCursorPositionSignal.connect(this, &MainWindow::onSetCursorPositionSignal);
    Core::appendConsoleTextSignal.connect(this, &MainWindow::onAppendConsoleTextSignal);
}

MainWindow::~MainWindow()
{
    writeSettings();
}

void
MainWindow::readSettings()
{
    QSettings settings("db", "mainwindow");

    resize(settings.value("size", QSize(720, 480)).toSize());
    move(settings.value("pos", QPoint(10, 10)).toPoint());

    if (settings.contains("splitter"))
    {
        QList<int> sizes;
        for (const auto &size : settings.value("splitter").toStringList())
            sizes.append(size.toInt());
        m_splitter->setSizes(sizes);
    }
    else
    {
        auto h = size().height();
        m_splitter->setSizes(QList<int>{ static_cast<int>(h*0.85), static_cast<int>(h*0.15) });
    }
}

void
MainWindow::writeSettings() const
{
    QSettings settings("db", "mainwindow");

    settings.setValue("pos", pos());
    settings.setValue("size", size());

    QStringList sizes;
    for (const auto &size : m_splitter->sizes())
        sizes << QString::number(size);
    settings.setValue("splitter", sizes);
}

void
MainWindow::createMenus()
{
    createFileMenu();
}

void
MainWindow::createFileMenu()
{
    auto filemenu = menuBar()->addMenu(tr("&File"));

//    // File actions
//    auto fileopenact = new QAction(tr("&Open..."), this);
//    fileopenact->setStatusTip(tr("Open an existing file"));
//    fileopenact->setShortcuts(QKeySequence::Open);
//    connect(fileopenact, SIGNAL(triggered()), this, SLOT(open()));
//    filemenu->addAction(fileopenact);

    filemenu->addSeparator();

    auto fileexitact = new QAction(tr("Quit"), this);
    fileexitact->setStatusTip(tr("Quit the application"));
    fileexitact->setShortcut(Qt::Key_Q);
    connect(fileexitact, SIGNAL(triggered()), this, SLOT(close()));
    filemenu->addAction(fileexitact);
}

void
MainWindow::createHotkeys()
{
//    auto normalModeAct = new QAction(this);
//    normalModeAct->setShortcut(Qt::Key_Escape);
//    connect(normalModeAct, SIGNAL(triggered()), this, SLOT(activateNormalMode()));
//    addAction(normalModeAct);
}

void
MainWindow::loadFile(const QString &filename)
{
    // read file into editor
    if (!filename.isEmpty())
    {
        QFile file(filename);
        QTextStream stream(&file);

        file.open(QFile::ReadOnly | QFile::Text);
        auto text = stream.readAll();
        m_editor->setText(text);

        auto numlines = text.count("\n");
        auto numdigits = numlines > 0 ? (int) log10((double) numlines) + 1 : 1;
        m_editor->setGutterWidth(numdigits);
    }
}

void
MainWindow::loadFileComplete()
{
    m_console->appendText("done.", true);
}

void
MainWindow::setCursorPosition(int row, int column)
{
    m_editor->setCursorPosition(row, column);
    m_editor->setBreakpointMarker(row);
}

void
MainWindow::appendConsoleText(const QString &text, bool newline)
{
    m_console->appendText(text, newline);
}

// wink signal handlers

void
MainWindow::onLoadFileSignal(const std::string &filename)
{
    QMetaObject::invokeMethod(this, "loadFile", Qt::QueuedConnection, Q_ARG(QString, QString::fromStdString(filename)));
}

void
MainWindow::onLoadFileCompleteSignal()
{
    QMetaObject::invokeMethod(this, "loadFileComplete", Qt::QueuedConnection);
}

void
MainWindow::onSetCursorPositionSignal(int row, int column)
{
    QMetaObject::invokeMethod(this, "setCursorPosition", Qt::QueuedConnection, Q_ARG(int, row), Q_ARG(int, column));
}

void
MainWindow::onAppendConsoleTextSignal(const std::string &text, bool newline)
{
    QMetaObject::invokeMethod(this, "appendConsoleText", Qt::QueuedConnection, Q_ARG(QString, QString::fromStdString(text)),
                                                                               Q_ARG(bool, newline));
}

}
