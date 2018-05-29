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
#include <QFileInfo>
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

    createDocks();
    createMenus();
    createToolbar();
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

    Core::Signal::loadFile.connect(this, &MainWindow::onLoadFileSignal);

    Core::Signal::appendConsoleText.connect(this, &MainWindow::onAppendConsoleText);
    Core::Signal::appendLogText.connect(this, &MainWindow::onAppendLogText);
    Core::Signal::appendOutputText.connect(this, &MainWindow::onAppendOutputText);

    Core::Signal::debuggerStateSet.connect(this, &MainWindow::onDebuggerStateSet);
}

MainWindow::~MainWindow()
{
    writeSettings();
}

void
MainWindow::quit()
{
    Core::gdb()->stop();
    close();
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
}

void
MainWindow::toggleConsoleTab()
{
    m_tabWidget->insertTab(0, m_consoleTab, tr("Console"));
}

void
MainWindow::toggleOutputTab()
{
    m_tabWidget->insertTab(1, m_outputTab, tr("Output"));
}

void
MainWindow::toggleGdbTab()
{
    std::vector<QWidget*> tabs;
    for (auto n=0; n < m_tabWidget->count(); ++n)
        tabs.push_back(m_tabWidget->widget(n));

    if (std::find(std::begin(tabs), std::end(tabs), m_gdbTab) != std::end(tabs))
        m_tabWidget->removeTab(2);
    else
        m_tabWidget->insertTab(2, m_gdbTab, tr("Gdb"));
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
    createViewMenu();
}

void
MainWindow::createToolbar()
{
    m_debugControls = new DebugControls(this);
    m_debugControls->setState(Core::State::Debugger::NONE);
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
    m_consoleTab = new Console(this, true);
    m_tabWidget->insertTab(0, m_consoleTab, tr("Console"));

    m_outputTab = new Console(this);
    m_tabWidget->insertTab(1, m_outputTab, tr("Output"));

    // hidden by default
    m_gdbTab = new Console(this);
    m_gdbTab->hide();

    // signal connections
    connect(m_tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    connect(m_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(switchTab(int)));

    addDockWidget(Qt::BottomDockWidgetArea, bottomdock);
}

void
MainWindow::createFileMenu()
{
    auto filemenu = menuBar()->addMenu(tr("&File"));

    filemenu->addSeparator();

    auto fileexitact = new QAction(tr("Quit"), this);
    fileexitact->setStatusTip(tr("Quit the application"));
    fileexitact->setShortcut(Qt::Key_Q);
    connect(fileexitact, SIGNAL(triggered()), this, SLOT(quit()));

    filemenu->addAction(fileexitact);
}

void
MainWindow::createViewMenu()
{
    std::vector<QWidget*> tabs;
    for (auto n=0; n < m_tabWidget->count(); ++n)
        tabs.push_back(m_tabWidget->widget(n));

    auto viewmenu = menuBar()->addMenu(tr("&View"));

    auto viewconsoletabact = new QAction(tr("Console"), this);
    viewconsoletabact->setStatusTip(tr("Show console tab"));
    connect(viewconsoletabact, SIGNAL(triggered()), this, SLOT(toggleConsoleTab()));
    viewconsoletabact->setCheckable(true);
    viewconsoletabact->setChecked(std::find(std::begin(tabs), std::end(tabs), m_consoleTab) != std::end(tabs));
    viewmenu->addAction(viewconsoletabact);

    auto viewoutputtabact = new QAction(tr("Output"), this);
    viewoutputtabact->setStatusTip(tr("Show output tab"));
    connect(viewoutputtabact, SIGNAL(triggered()), this, SLOT(toggleOutputTab()));
    viewoutputtabact->setCheckable(true);
    viewoutputtabact->setChecked(std::find(std::begin(tabs), std::end(tabs), m_outputTab) != std::end(tabs));
    viewmenu->addAction(viewoutputtabact);

    auto viewgdbtabact = new QAction(tr("Gdb"), this);
    viewgdbtabact->setStatusTip(tr("Show Gdb tab"));
    connect(viewgdbtabact, SIGNAL(triggered()), this, SLOT(toggleGdbTab()));
    viewgdbtabact->setCheckable(true);
    viewgdbtabact->setChecked(std::find(std::begin(tabs), std::end(tabs), m_gdbTab) != std::end(tabs));
    viewmenu->addAction(viewgdbtabact);
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
MainWindow::onLoadFileSignal(const std::string &filename)
{
    QFileInfo checkfile(QString::fromStdString(filename));
    if (checkfile.exists() && checkfile.isFile())
    {
        setWindowTitle("db " + QString::fromStdString(filename));
    }
}

void
MainWindow::onAppendConsoleText(const std::string &text)
{
    QMetaObject::invokeMethod(m_consoleTab, "appendText", Qt::QueuedConnection, Q_ARG(QString, QString::fromStdString(text)));
}

void
MainWindow::onAppendLogText(const std::string &text)
{
    QMetaObject::invokeMethod(m_gdbTab, "appendText", Qt::QueuedConnection, Q_ARG(QString, QString::fromStdString(text)));
}

void
MainWindow::onAppendOutputText(const std::string &text)
{
    QMetaObject::invokeMethod(m_outputTab, "appendText", Qt::QueuedConnection, Q_ARG(QString, QString::fromStdString(text)));
}

void
MainWindow::onDebuggerStateSet(Core::State::Debugger state)
{
    m_debugControls->setState(state);
}

}
