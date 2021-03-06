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
#include <QTimer>
#include <QKeyEvent>
#include <QEvent>

#include "fmt/format.h"

#include "core/signals.h"
#include "core/global.h"

#include "gdb/global.h"
#include "gdb/commands.h"

#include "editor.h"
#include "tabWidget.h"
#include "output.h"
#include "coloredOutput.h"
#include "gdbConsole.h"
#include "callStack.h"
#include "breakPoints.h"
#include "variables.h"
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
    m_editor->setObjectName("maineditor");

    setCentralWidget(m_editor);

    // restore gui settings
    readSettings();

    // toolbar must be created before menus (shared actions)
    createDocks();
    createToolbar();
    createMenus();
    createHotkeys();
    createStatusbar();

    // ============
    auto focustimer = new QTimer(this);
    focustimer->setInterval(500);
    focustimer->start();
//    connect(focustimer, &QTimer::timeout, [&]() { std::cout << "focusWidget: " << (focusWidget() ? focusWidget()->objectName().toStdString() : "N/A") << std::endl; } );
    // ============

    // signal handlers
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

    Core::Signals::loadEditorSource.connect([this](const std::string &t)
    {
        QMetaObject::invokeMethod(this, "onLoadEditorSource", Qt::QueuedConnection, Q_ARG(QString, QString::fromStdString(t)));
    });

    Core::Signals::debuggerStateUpdated.connect([this]()
    {
        QMetaObject::invokeMethod(this, "onDebuggerStateUpdated", Qt::QueuedConnection);
    });
}

MainWindow::~MainWindow()
{
    // save gui settings
    writeSettings();
}

bool
MainWindow::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        auto keyevent = static_cast<QKeyEvent *>(event);
        if ((keyevent->modifiers() & Qt::ControlModifier))
        {
            if (keyevent->key() == Qt::Key_Right)
            {
                auto count = m_bottomTabWidget->count();
                auto index = m_bottomTabWidget->currentIndex();
                m_bottomTabWidget->setCurrentIndex((index+1) % count);
                return true;
            }
            else if (keyevent->key() == Qt::Key_Left)
            {
                auto count = m_bottomTabWidget->count();
                auto index = m_bottomTabWidget->currentIndex();
                m_bottomTabWidget->setCurrentIndex((index+count-1) % count);
                return true;
            }
        }
    }

    return QMainWindow::eventFilter(object, event);
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

    // remove or insert tab depending on whether it currently exists
    auto it = std::find(std::begin(tabs), std::end(tabs), tab);
    if (it != std::end(tabs))
        m_bottomTabWidget->removeTab(it - std::begin(tabs));
    else
        m_bottomTabWidget->insertTab(m_bottomTabWidget->count(), tab, tab->objectName());
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
        if (child->property("istab").isValid())
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
    m_bottomTabWidget = new TabWidget(this);
    m_bottomTabWidget->setObjectName("tabwidget");
    m_bottomTabWidget->setTabsClosable(true);
    m_bottomTabWidget->setMovable(true);
    m_bottomTabWidget->setUsesScrollButtons(false);

    // lambda to return index of tab from settings or default value if not set
    auto settingsIndex = [&](const QWidget *tab)
    {
        auto key = "MainWindow/BottomDock/TabOrder/" + tab->objectName();
        return m_settings->contains(key) ? m_settings->value(key).toInt() : -1;
    };

    std::vector<QWidget*> tabs;

    // add individual tab widgets
    m_gdbConsoleTab = new GdbConsole(this);
    m_gdbConsoleTab->setObjectName("Gdb");
    m_gdbConsoleTab->setProperty("istab", true);
    tabs.push_back(m_gdbConsoleTab);

    m_programOutputTab = new Output(this);
    m_programOutputTab->setObjectName("Output");
    m_programOutputTab->setProperty("istab", true);
    tabs.push_back(m_programOutputTab);

    m_callStackTab = new CallStack(this);
    m_callStackTab->setObjectName("CallStack");
    m_callStackTab->setProperty("istab", true);
    tabs.push_back(m_callStackTab);

    m_breakPointsTab = new BreakPoints(this);
    m_breakPointsTab->setObjectName("BreakPoints");
    m_breakPointsTab->setProperty("istab", true);
    tabs.push_back(m_breakPointsTab);

    m_consoleOutputTab = new ColoredOutput(this);
    m_consoleOutputTab->setObjectName("Console");
    m_consoleOutputTab->setProperty("istab", true);
    tabs.push_back(m_consoleOutputTab);

    m_variablesTab = new Variables(this);
    m_variablesTab->setObjectName(tr("Variables"));
    m_variablesTab->setProperty("istab", true);
    tabs.push_back(m_variablesTab);

    // sort tabs by settings index value
    std::sort(std::begin(tabs), std::end(tabs), [&](QWidget *a, QWidget *b) { return settingsIndex(a) < settingsIndex(b); });

    // add created tab widgets
    for (const auto &tab : tabs)
    {
        if (settingsIndex(tab) != -1)
            m_bottomTabWidget->addTab(tab, tab->objectName());
    }

    // signal connections
    connect(m_bottomTabWidget, &QTabWidget::tabCloseRequested, [&](int index){ closeBottomTab(index); });
    connect(m_bottomTabWidget, &QTabWidget::currentChanged, [&](int index){ switchBottomTab(index); });

    bottomdock->setWidget(m_bottomTabWidget);
    addDockWidget(Qt::BottomDockWidgetArea, bottomdock);

    // set current tab from settings
    m_bottomTabWidget->setCurrentIndex(m_settings->value("MainWindow/BottomDock/CurrentTab", 0).toInt());
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

    // lambda to create action from tab widget and add to view menu
    auto addAction = [&](QWidget *tabwidget)
    {
        auto name = tabwidget->objectName();
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
    addAction(m_variablesTab);
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
MainWindow::onLoadEditorSource(const QString &filename)
{
    QFileInfo checkfile(filename);
    if (checkfile.exists() && checkfile.isFile())
    {
        setWindowTitle("db " + filename);
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
        Core::Signals::uiRealized.emit();
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
