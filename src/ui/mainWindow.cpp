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

#include "core/signals.h"
#include "core/global.h"

#include "gdb/global.h"
#include "gdb/commands.h"

#include "editor.h"
#include "output.h"
#include "coloredOutput.h"
#include "gdbConsole.h"
#include "callStack.h"
#include "breakPoints.h"
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

    // toolbar must be created before menus (shared actions)
    createDocks();
    createToolbar();
    createMenus();
    createHotkeys();
    createStatusbar();

    // signal handlers
    Core::Signals::loadEditorSource.connect(this, &MainWindow::onLoadFileSignal);
    Core::Signals::debuggerStateUpdated.connect(this, &MainWindow::onDebuggerStateUpdated);

    Core::Signals::appendConsoleText.connect([this](const std::string &t)
    {
        QMetaObject::invokeMethod(m_gdbConsoleTab, "appendText", Qt::QueuedConnection, Q_ARG(QString, QString::fromStdString(t)));
    });

    Core::Signals::appendOutputText.connect([this](const std::string &t)
    {
        QMetaObject::invokeMethod(m_programOutputTab, "appendText", Qt::QueuedConnection, Q_ARG(QString, QString::fromStdString(t)));
    });

    Core::Signals::appendStdoutText.connect([this](const std::string &t)
    {
        QMetaObject::invokeMethod(m_consoleOutputTab, "appendText", Qt::QueuedConnection, Q_ARG(QString, QString::fromStdString(t)), Q_ARG(QColor, Qt::green));
    });

    Core::Signals::appendStderrText.connect([this](const std::string &t)
    {
        QMetaObject::invokeMethod(m_consoleOutputTab, "appendText", Qt::QueuedConnection, Q_ARG(QString, QString::fromStdString(t)), Q_ARG(QColor, Qt::red));
    });

    Core::Signals::setStatusbarText.connect([this](const std::string &t)
    {
        QMetaObject::invokeMethod(statusBar(), "showMessage", Qt::QueuedConnection, Q_ARG(QString, QString::fromStdString(t)));
    });

    Core::Signals::requestQuit.connect([this]()
    {
        QMetaObject::invokeMethod(this, "quit", Qt::QueuedConnection);
    });
}

MainWindow::~MainWindow()
{
    // save gui settings
    writeSettings();
}

void
MainWindow::closeBottomTab(int index)
{
    // retrieve action pointer from tab widget and use it to uncheck the menu item
    const auto &tab = m_bottomTabWidget->widget(index);
    const auto action = tab->property("action").value<QAction*>();
    action->setChecked(false);

    m_bottomTabWidget->removeTab(index);
}

void
MainWindow::switchBottomTab(int index)
{
    auto console = qobject_cast<GdbConsole*>(m_bottomTabWidget->widget(index));
    if (console)
    {
//        console->verticalScrollBar()->setValue(console->verticalScrollBar()->maximum());
    }
}

void
MainWindow::toggleTab(QWidget *tab)
{
    // build list of existing tabs
    std::vector<QWidget*> tabs;
    for (auto n=0; n < m_bottomTabWidget->count(); ++n)
        tabs.push_back(m_bottomTabWidget->widget(n));

    // remove or insert tab depending on whether it already currently exists
    auto it = std::find(std::begin(tabs), std::end(tabs), tab);
    if (it != std::end(tabs))
        m_bottomTabWidget->removeTab(it - std::begin(tabs));
    else
        m_bottomTabWidget->insertTab(m_bottomTabWidget->count(), tab, tab->property("tabname").toString());
}

void
MainWindow::readSettings()
{
    m_settings = new QSettings(this);

    restoreGeometry(m_settings->value("MainWindow/Geometry").toByteArray());
    restoreState(m_settings->value("MainWindow/State").toByteArray());
}

void
MainWindow::writeSettings() const
{
    m_settings->setValue("MainWindow/State", saveState());
    m_settings->setValue("MainWindow/Geometry", saveGeometry());

    // save bottom dock's current tab
    m_settings->setValue("MainWindow/BottomDock/CurrentTab", m_bottomTabWidget->currentIndex());

    // save index of bottomdock's tabs
    for (const auto child : findChildren<QWidget *>())
    {
        auto tabname = child->property("tabname");
        if (tabname.isValid())
        {
            m_settings->setValue("MainWindow/BottomDock/TabOrder/" + child->objectName(), m_bottomTabWidget->indexOf(child));
        }
    }
}

void
MainWindow::createMenus()
{
    createFileMenu();
    createDebugMenu();
    createViewMenu();
}

void
MainWindow::createToolbar()
{
    m_debugControls = new DebugControls(this);
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
    m_bottomTabWidget = new QTabWidget(this);
    m_bottomTabWidget->setObjectName("tabwidget");
    m_bottomTabWidget->setTabsClosable(true);
    m_bottomTabWidget->setMovable(true);

    // lambda to return index of tab from settings or default value if not set
    auto getSettingsIndex = [&](const QWidget *tab)
    {
        auto key = "MainWindow/BottomDock/TabOrder/" + tab->objectName();
//        std::cout << tab->objectName().toStdString() << " " << key.toStdString() << " " << (m_settings->contains(key) ? m_settings->value(key).toInt() : def) << std::endl;
        return m_settings->contains(key) ? m_settings->value(key).toInt() : -1;
    };

    // lambda to return index of tab from name
    auto getIndex = [&](const QString &name)
    {
        auto page = m_bottomTabWidget->findChild<QWidget *>(name);
        return m_bottomTabWidget->indexOf(page);
    };

//    int index;
//    (void)index;
    (void)getIndex;

    // add individual tab widgets
    m_gdbConsoleTab = new GdbConsole(this, true);
    m_gdbConsoleTab->setObjectName("Gdb");
    m_gdbConsoleTab->setProperty("tabname", tr("Gdb"));

//    index = getIndex(m_gdbConsoleTab, 0);
//    if (index >= 0)
//        m_bottomTabWidget->insertTab(index, m_gdbConsoleTab, m_gdbConsoleTab->property("tabname").toString());

    m_bottomTabWidget->addTab(m_gdbConsoleTab, m_gdbConsoleTab->property("tabname").toString());


    m_programOutputTab = new Output(this);
    m_programOutputTab->setObjectName("Output");
    m_programOutputTab->setProperty("tabname", "Output");
//    index = getIndex(m_programOutputTab, 1);
//    if (index >= 0)
//        m_bottomTabWidget->insertTab(index, m_programOutputTab, m_programOutputTab->property("tabname").toString());

    m_bottomTabWidget->addTab(m_programOutputTab, m_programOutputTab->property("tabname").toString());

    m_callStackTab = new CallStack(this);
    m_callStackTab->setObjectName("CallStack");
    m_callStackTab->setProperty("tabname", tr("Call Stack"));
//    index = getIndex(m_callStackTab, 2);
//    if (index >= 0)
//        m_bottomTabWidget->insertTab(index, m_callStackTab, m_callStackTab->property("tabname").toString());

    m_bottomTabWidget->addTab(m_callStackTab, m_callStackTab->property("tabname").toString());

    m_breakPointsTab = new BreakPoints(this);
    m_breakPointsTab->setObjectName("BreakPoints");
    m_breakPointsTab->setProperty("tabname", tr("Break Points"));
//    index = getIndex(m_breakPointsTab, 3);
//    if (index >= 0)
//        m_bottomTabWidget->insertTab(index, m_breakPointsTab, m_breakPointsTab->property("tabname").toString());

    m_bottomTabWidget->addTab(m_breakPointsTab, m_breakPointsTab->property("tabname").toString());

    m_consoleOutputTab = new ColoredOutput(this);
    m_consoleOutputTab->setObjectName("Console");
    m_consoleOutputTab->setProperty("tabname", tr("Console"));
//    index = getIndex(m_consoleOutputTab, 4);
//    if (index >= 0)
//        m_bottomTabWidget->insertTab(index, m_consoleOutputTab, m_consoleOutputTab->property("tabname").toString());

    m_bottomTabWidget->addTab(m_consoleOutputTab, m_consoleOutputTab->property("tabname").toString());


    //////////////

    for (const auto child : m_bottomTabWidget->findChildren<QWidget *>())
    {
        auto tabname = child->property("tabname");
        if (tabname.isValid())
        {
            auto oldindex = m_bottomTabWidget->indexOf(child);
            auto newindex = getSettingsIndex(child);
            if (newindex != -1 && newindex != oldindex)
            {
                m_bottomTabWidget->tabBar()->move(oldindex, newindex);
                std::cout << "moving tab " << child->objectName().toStdString() << " from " << oldindex << " to " << newindex << std::endl;
//            m_settings->setValue("MainWindow/BottomDock/TabOrder/" + child->objectName(), m_bottomTabWidget->indexOf(child));
            }
        }
    }

    //    m_bottomTabWidget->tabBar()->move();

    // signal connections
    connect(m_bottomTabWidget, &QTabWidget::tabCloseRequested, [&](int index){ closeBottomTab(index); });
    connect(m_bottomTabWidget, &QTabWidget::currentChanged, [&](int index){ switchBottomTab(index); });

    bottomdock->setWidget(m_bottomTabWidget);
    addDockWidget(Qt::BottomDockWidgetArea, bottomdock);

    // restore current index from settings
    m_bottomTabWidget->setCurrentIndex(m_settings->value("MainWindow/BottomDock/TabIndex", 0).toInt());
}

void
MainWindow::createFileMenu()
{
    auto filemenu = menuBar()->addMenu(tr("&File"));
    filemenu->setObjectName("file");

    filemenu->addSeparator();

    auto fileexitact = new QAction(tr("Quit"), this);
    fileexitact->setStatusTip(tr("Quit the application"));
    fileexitact->setShortcut(Qt::CTRL + Qt::Key_Q);
    connect(fileexitact, &QAction::triggered, [&](){ quit(); });

    filemenu->addAction(fileexitact);
}

void
MainWindow::createDebugMenu()
{
    auto debugmenu = menuBar()->addMenu(tr("&Debug"));
    debugmenu->setObjectName("debug");

    // add actions from debug controls bar to debug menu
    for (const auto action : m_debugControls->actions())
    {
        debugmenu->addAction(action);
    }
}

void
MainWindow::createViewMenu()
{
    // build list of currently present tabs
    std::vector<QWidget*> tabs;
    for (auto n=0; n < m_bottomTabWidget->count(); ++n)
        tabs.push_back(m_bottomTabWidget->widget(n));

    auto viewmenu = menuBar()->addMenu(tr("&View"));
    viewmenu->setObjectName("view");

    // function to create action from tab widget and add to view menu
    auto addAction = [&](QWidget *tabwidget)
    {
        auto name = tabwidget->property("tabname").toString();
        auto action = new QAction(name, this);
        action->setStatusTip(tr("Show ") + name + tr(" tab"));
        action->setCheckable(true);
        action->setChecked(std::find(std::begin(tabs), std::end(tabs), tabwidget) != std::end(tabs));
        connect(action, &QAction::triggered, [=] { toggleTab(tabwidget); });
        tabwidget->setProperty("action", QVariant::fromValue(action));
        viewmenu->addAction(action);
    };

    addAction(m_gdbConsoleTab);
    addAction(m_consoleOutputTab);
    addAction(m_programOutputTab);
    addAction(m_callStackTab);
    addAction(m_breakPointsTab);
}

void
MainWindow::createStatusbar()
{
    m_statusIcon = new QLabel(this);
    m_statusIcon->setScaledContents(true);

    statusBar()->insertPermanentWidget(0, m_statusIcon);
    statusBar()->setSizeGripEnabled(false);
    statusBar()->setStyleSheet("QStatusBar {min-height: 28; }");

    statusBar()->show();
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

// signal handlers

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
MainWindow::onDebuggerStateUpdated()
{
    auto state = Core::state()->debuggerState();

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

void
MainWindow::showEvent(QShowEvent* event)
{
    static bool first = true;

    QMainWindow::showEvent(event);

    if (first)
    {
        Core::Signals::UiRealized.emit();
        first = false;
    }
}

void
MainWindow::quit()
{
    Gdb::commands()->stop();
    close();
}

}
