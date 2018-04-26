/*
 * mainWindow.cpp
 *
 *  Created on: Feb 6, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <sstream>

#include <QFile>
#include <QSettings>
#include <QMenu>
#include <QMenuBar>
#include <QSplitter>
#include <QToolBar>

#include "core/signal.h"
#include "core/global.h"

#include "gdb/commands.h"

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
    createToolbar();
    createHotkeys();
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
MainWindow::createToolbar()
{
    auto toolbar = addToolBar(tr("Debug"));
    toolbar->setAllowedAreas(Qt::TopToolBarArea);

    auto runact = new QAction(QIcon(":/img/run"), tr("Run/Continue"), this);
    runact->setStatusTip(tr("Start/continue debugging"));
    runact->setShortcut(Qt::Key_R);
    connect(runact, SIGNAL(triggered()), this, SLOT(run()));

    auto pauseact = new QAction(QIcon(":/img/pause"), tr("Pause"), this);
    pauseact->setStatusTip(tr("Pause execution"));
    pauseact->setShortcut(Qt::Key_P);
    connect(pauseact, SIGNAL(triggered()), this, SLOT(pause()));

    auto stepoveract = new QAction(QIcon(":/img/stepover"), tr("Step over"), this);
    stepoveract->setStatusTip(tr("Step over"));
    stepoveract->setShortcut(Qt::Key_N);
    connect(stepoveract, SIGNAL(triggered()), this, SLOT(stepover()));

    auto stepintoact = new QAction(QIcon(":/img/stepinto"), tr("Step into"), this);
    stepintoact->setStatusTip(tr("Step into"));
    stepintoact->setShortcut(Qt::Key_S);
    connect(stepintoact, SIGNAL(triggered()), this, SLOT(stepinto()));

    auto stepoutact = new QAction(QIcon(":/img/stepout"), tr("Step out"), this);
    stepoutact->setStatusTip(tr("Step out"));
//    stepoutact->setShortcut(Qt::Key_R);
    connect(stepoutact, SIGNAL(triggered()), this, SLOT(stepout()));

    toolbar->addAction(runact);
    toolbar->addAction(pauseact);
    toolbar->addAction(stepoveract);
    toolbar->addAction(stepintoact);
    toolbar->addAction(stepoutact);
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
}

}
