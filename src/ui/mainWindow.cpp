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
#include <QSettings>
#include <QMenu>
#include <QMenuBar>
#include <QSplitter>
#include <QToolBar>
#include <QDockWidget>
#include <QShortcut>
#include <QScrollBar>

#include "core/signal.h"
#include "core/global.h"

#include "gdb/commands.h"

#include "editor.h"
#include "console.h"
#include "debugControls.h"
#include "mainWindow.h"

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
//    m_splitter = new QSplitter(this);
//    m_splitter->setOrientation(Qt::Vertical);
//    m_splitter->setObjectName("splitter");
//    m_splitter->setContentsMargins(0, 0, 0, 0);

    // main editor window
    m_editor = new Editor(this);

    // add editor and tabwidget to main
//    m_splitter->addWidget(m_editor);
//    m_splitter->addWidget(m_tabWidget);

    setCentralWidget(m_editor);

    // settings
    readSettings();

    createMenus();
    createToolbar();
    createDocks();
    createHotkeys();

/*
    auto dockwidget1 = new QDockWidget(tr("Tab1"), this);
    dockwidget1->setObjectName("dw1");
    dockwidget1->setMinimumSize({0, 0});
    dockwidget1->setTitleBarWidget(new QWidget(this));

    auto dockwidget2 = new QDockWidget(tr("Tab2"), this);
    dockwidget2->setObjectName("dw2");
    dockwidget2->setMinimumSize({0, 0});
    dockwidget2->setTitleBarWidget(new QWidget(this));

    addDockWidget(Qt::LeftDockWidgetArea , dockwidget1);
    addDockWidget(Qt::LeftDockWidgetArea , dockwidget2);
//    tabifyDockWidget(dockwidget1,dockwidget2);
*/

    Core::Signal::appendConsoleText.connect(this, &MainWindow::onAppendConsoleText);
    Core::Signal::appendLogText.connect(this, &MainWindow::onAppendLogText);
    Core::Signal::appendOutputText.connect(this, &MainWindow::onAppendOutputText);

    Core::Signal::setDebuggerState.connect(this, &MainWindow::onSetDebuggerState);
}

MainWindow::~MainWindow()
{
    writeSettings();
}

void
MainWindow::run()
{
    Core::gdb()->run();
}

void
MainWindow::pause()
{
    Core::gdb()->pause();
}

void
MainWindow::stop()
{
    Core::gdb()->stop();
}

void
MainWindow::reload()
{
    Core::gdb()->reload();
}

void
MainWindow::stepover()
{
    Core::gdb()->stepover();
}

void
MainWindow::stepinto()
{
    Core::gdb()->stepinto();
}

void
MainWindow::stepout()
{
    Core::gdb()->stepout();
}

void
MainWindow::closeTab(int index)
{
    m_tabWidget->removeTab(index);
}

void
MainWindow::switchTab(int index)
{
    auto console = qobject_cast<Console*>(m_tabWidget->widget(index));
    if (console)
    {
        console->verticalScrollBar()->setValue(console->verticalScrollBar()->maximum());
        std::cout << index << " " << console->verticalScrollBar()->maximum() << std::endl;
    }
//    qobject_cast<Console*>(m_tabWidget->widget(index))->ensureCursorVisible();
//    std::cout << "st " << index << std::endl;
}

void
MainWindow::readSettings()
{
    QSettings settings("db", "mainwindow");

    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("state").toByteArray());

//    resize(settings.value("size", QSize(720, 480)).toSize());
//    move(settings.value("pos", QPoint(10, 10)).toPoint());
//
//    if (settings.contains("splitter"))
//    {
//        QList<int> sizes;
//        for (const auto &size : settings.value("splitter").toStringList())
//            sizes.append(size.toInt());
//        m_splitter->setSizes(sizes);
//    }
//    else
//    {
//        auto h = size().height();
//        m_splitter->setSizes(QList<int>{ static_cast<int>(h*0.85), static_cast<int>(h*0.15) });
//    }
}

void
MainWindow::writeSettings() const
{
//    QSettings settings("db", "mainwindow");
//
//    settings.setValue("pos", pos());
//    settings.setValue("size", size());
//
//    QStringList sizes;
//    for (const auto &size : m_splitter->sizes())
//        sizes << QString::number(size);
//    settings.setValue("splitter", sizes);

    QSettings settings("db", "mainwindow");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
}

void
MainWindow::createMenus()
{
    createFileMenu();
}

void
MainWindow::createToolbar()
{
    m_debugControls = new DebugControls(this);
    m_debugControls->setState(Core::State::Debugger::LAUNCHED);
    addToolBar(m_debugControls);
}

void
MainWindow::createDocks()
{
    auto bottomdock = new QDockWidget(tr("bottomdock"), this);
    bottomdock->setObjectName(tr("bottomdock"));
    bottomdock->setAllowedAreas(Qt::BottomDockWidgetArea);

    // to hide the title bar completely must replace the default widget with a generic one
    auto titlewidget = new QWidget(this);
    bottomdock->setTitleBarWidget(titlewidget);

    // tabs window
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setTabsClosable(true);
    m_tabWidget->setMovable(true);

    bottomdock->setWidget(m_tabWidget);

    // tabs within tab window
    m_console = new Console(this);
    m_tabWidget->addTab(m_console, tr("Console"));

    m_log = new Console(this);
    m_tabWidget->addTab(m_log, tr("Log"));

    m_output = new Console(this);
    m_tabWidget->addTab(m_output, tr("Output"));

    // signal connections
    connect(m_tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    connect(m_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(switchTab(int)));


    addDockWidget(Qt::BottomDockWidgetArea, bottomdock);
}

void
MainWindow::createFileMenu()
{
    auto filemenu = menuBar()->addMenu(tr("&File"));

//    // File actions

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
    auto zoomin = new QShortcut(Qt::CTRL + Qt::Key_Equal, this);
    connect(zoomin, &QShortcut::activated, [&](){ m_editor->zoomInText(); });

    auto zoomout = new QShortcut(Qt::CTRL + Qt::Key_Minus, this);
    connect(zoomout, &QShortcut::activated, [&](){ m_editor->zoomOutText(); });

    auto zoomreset = new QShortcut(Qt::CTRL + Qt::Key_0, this);
    connect(zoomreset, &QShortcut::activated, [&](){ m_editor->zoomResetText(); });
}

// wink signal handlers

void
MainWindow::onAppendConsoleText(const std::string &text)
{
    QMetaObject::invokeMethod(m_console, "appendText", Qt::QueuedConnection, Q_ARG(QString, QString::fromStdString(text)));
}

void
MainWindow::onAppendLogText(const std::string &text)
{
    QMetaObject::invokeMethod(m_log, "appendText", Qt::QueuedConnection, Q_ARG(QString, QString::fromStdString(text)));
}

void
MainWindow::onAppendOutputText(const std::string &text)
{
    QMetaObject::invokeMethod(m_output, "appendText", Qt::QueuedConnection, Q_ARG(QString, QString::fromStdString(text)));
}

void
MainWindow::onSetDebuggerState(Core::State::Debugger state)
{
    m_debugControls->setState(state);
}

}
