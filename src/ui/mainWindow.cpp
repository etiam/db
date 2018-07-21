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
#include <QStatusBar>
#include <QLabel>

#include "core/signal.h"
#include "core/global.h"

#include "gdb/commands.h"

#include "editor.h"
#include "output.h"
#include "console.h"
#include "callStack.h"
#include "debugControls.h"
#include "mainWindow.h"

Q_DECLARE_METATYPE(QList<int>)
Q_DECLARE_METATYPE(QAction*);

namespace Ui
{

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    qRegisterMetaTypeStreamOperators<QList<int>>("QList<int>");

    setObjectName("mainwindow");
    setWindowTitle("db");

    // main editor window
    m_editor = new Editor(this);
    setCentralWidget(m_editor);

    // restore gui settings
    readSettings();

    createDocks();
    createMenus();
    createToolbar();
    createHotkeys();
    createStatusbar();

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

    // wink signals
    Core::Signal::loadFile.connect(this, &MainWindow::onLoadFileSignal);

    Core::Signal::appendConsoleText.connect(this, &MainWindow::onAppendConsoleText);
    Core::Signal::appendLogText.connect(this, &MainWindow::onAppendLogText);
    Core::Signal::appendOutputText.connect(this, &MainWindow::onAppendOutputText);

    Core::Signal::debuggerStateUpdated.connect(this, &MainWindow::onDebuggerStateUpdated);
}

MainWindow::~MainWindow()
{
    // save gui settings
    writeSettings();
}

void
MainWindow::quit()
{
    Core::gdb()->stop();
    close();
}

template<typename E>
constexpr auto toIntegral(E e) -> typename std::underlying_type<E>::type
{
   return static_cast<typename std::underlying_type<E>::type>(e);
}

void
MainWindow::run()
{
    // this will make the current location marker disappear
    Core::Signal::clearCurrentLocation();

    if (Core::state()->debuggerState() == Core::State::Debugger::PAUSED)
        Core::gdb()->cont();
    else
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
    // retrieve action pointer from tab widget and use it to uncheck the menu item
    const auto &tab = m_tabWidget->widget(index);
    const auto action = tab->property("action").value<QAction*>();
    action->setChecked(false);

    m_tabWidget->removeTab(index);
}

void
MainWindow::switchTab(int index)
{
    auto console = qobject_cast<Console*>(m_tabWidget->widget(index));
    if (console)
    {
        console->verticalScrollBar()->setValue(console->verticalScrollBar()->maximum());
    }
}

void
MainWindow::toggleTab(QWidget *tab)
{
    // build list of existing tabs
    std::vector<QWidget*> tabs;
    for (auto n=0; n < m_tabWidget->count(); ++n)
        tabs.push_back(m_tabWidget->widget(n));

    // remove or insert tab depending on whether it already currently exists
    auto it = std::find(std::begin(tabs), std::end(tabs), tab);
    if (it != std::end(tabs))
        m_tabWidget->removeTab(it - std::begin(tabs));
    else
        m_tabWidget->insertTab(m_tabWidget->count(), tab, tab->property("tabname").toString());
}

void
MainWindow::readSettings()
{
    QSettings settings;

    restoreGeometry(settings.value("MainWindow/Geometry").toByteArray());
    restoreState(settings.value("MainWindow/State").toByteArray());
}

void
MainWindow::writeSettings() const
{
    QSettings settings;

    settings.setValue("MainWindow/State", saveState());
    settings.setValue("MainWindow/Geometry", saveGeometry());
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
    auto bottomdock = new QDockWidget("bottomdock", this);
    bottomdock->setObjectName("bottomdock");
    bottomdock->setAllowedAreas(Qt::BottomDockWidgetArea);

    // to hide the title bar completely we must replace the default widget with a QWidget
    auto titlewidget = new QWidget(this);
    bottomdock->setTitleBarWidget(titlewidget);

    // tabs window
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setObjectName("tabwidget");
    m_tabWidget->setTabsClosable(true);
    m_tabWidget->setMovable(true);

    bottomdock->setWidget(m_tabWidget);

    // tabs within tab window
    m_consoleTab = new Console(this, true);
    m_consoleTab->setObjectName("console");
    m_consoleTab->setProperty("tabname", tr("Console"));
    m_tabWidget->insertTab(0, m_consoleTab, tr("Console"));

    m_programOutputTab = new Output(this);
    m_programOutputTab->setObjectName("programoutput");
    m_programOutputTab->setProperty("tabname", "Output");
    m_tabWidget->insertTab(1, m_programOutputTab, tr("Output"));

    m_callStackTab = new CallStack(this);
    m_callStackTab->setObjectName("callstack");
    m_callStackTab->setProperty("tabname", tr("Call Stack"));
    m_tabWidget->insertTab(2, m_callStackTab, tr("Call Stack"));

    // debugger tab is hidden by default
    m_debuggerOutputTab = new Output(this);
    m_debuggerOutputTab->setObjectName("debugoutput");
    m_debuggerOutputTab->setProperty("tabname", tr("Gdb"));
    m_debuggerOutputTab->hide();

    // signal connections
    connect(m_tabWidget, &QTabWidget::tabCloseRequested, [&](int index){ closeTab(index); });
    connect(m_tabWidget, &QTabWidget::currentChanged, [&](int index){ switchTab(index); });

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
    connect(fileexitact, &QAction::triggered, [&](){ quit(); });

    filemenu->addAction(fileexitact);
}

void
MainWindow::createViewMenu()
{
    // build list of currently present tabs
    std::vector<QWidget*> tabs;
    for (auto n=0; n < m_tabWidget->count(); ++n)
        tabs.push_back(m_tabWidget->widget(n));

    auto viewmenu = menuBar()->addMenu(tr("&View"));

    // function to create action from tab widget and add to view menu
    auto addAction = [&](QWidget *tabwidget)
    {
        auto name = tabwidget->property("tabname").toString();
        auto action = new QAction(name, this);
        action->setStatusTip(tr("Show") + name + tr(" tab"));
        action->setCheckable(true);
        action->setChecked(std::find(std::begin(tabs), std::end(tabs), tabwidget) != std::end(tabs));
        connect(action, &QAction::triggered, [=] { toggleTab(tabwidget); });
        tabwidget->setProperty("action", QVariant::fromValue(action));
        viewmenu->addAction(action);
    };

    addAction(m_consoleTab);
    addAction(m_programOutputTab);
    addAction(m_debuggerOutputTab);
    addAction(m_callStackTab);
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

void
MainWindow::createStatusbar()
{
    m_statusIcon = new QLabel(this);
    m_statusIcon->setScaledContents(true);

    statusBar()->insertPermanentWidget(0, m_statusIcon);
    statusBar()->setSizeGripEnabled(false);
    statusBar()->show();
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
    QMetaObject::invokeMethod(m_debuggerOutputTab, "appendText", Qt::QueuedConnection, Q_ARG(QString, QString::fromStdString(text)));
}

void
MainWindow::onAppendOutputText(const std::string &text)
{
    QMetaObject::invokeMethod(m_programOutputTab, "appendText", Qt::QueuedConnection, Q_ARG(QString, QString::fromStdString(text)));
}

void
MainWindow::onDebuggerStateUpdated()
{
    auto state = Core::state()->debuggerState();

    m_debugControls->setState(state);

    switch (state)
    {
        case Core::State::Debugger::LOADED :
            m_statusIcon->setPixmap(QPixmap(":/img/loaded"));
            break;

        case Core::State::Debugger::RUNNING :
            m_statusIcon->setPixmap(QPixmap(":/img/running"));
            break;

        case Core::State::Debugger::PAUSED :
            m_statusIcon->setPixmap(QPixmap(":/img/paused"));
            break;

        default:
            m_statusIcon->setPixmap(QPixmap(24, 24));
            break;
    }
}

}
