#include <iostream>
#include <QFile>
#include <QTextStream>
#include <QWidget>
#include <QVBoxLayout>
#include <QSettings>
#include <QMenu>
#include <QMenuBar>

#include "editor.h"
#include "console.h"
#include "mainwindow.h"

namespace Ui
{

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setObjectName("mainwindow");
    setWindowTitle("db");

    // gui setup
    auto centralwidget = new QWidget(this);
    centralwidget->setObjectName("centralWidget");

    auto centralwidgetlayout = new QVBoxLayout(centralwidget);
    centralwidgetlayout->setSpacing(0);
    centralwidgetlayout->setObjectName("centralwidgetlayout");
    centralwidgetlayout->setContentsMargins(0, 0, 0, 0);

    auto editor = new Editor(this);
    auto console = new Console(this);

    centralwidgetlayout->addWidget(editor, 1);
    centralwidgetlayout->addWidget(console, 0);
    setCentralWidget(centralwidget);

    // settings
    readSettings();

    createMenus();
    createHotkeys();
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
}

void
MainWindow::writeSettings()
{
    QSettings settings("db", "mainwindow");

    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

void
MainWindow::createMenus()
{
    createFileMenu();
//    createViewMenu();
//    createControlMenu();
//    createWindowMenu();
//    createPreferencesMenu();
//    createHelpMenu();
}

void
MainWindow::createFileMenu()
{
    auto filemenu = menuBar()->addMenu(tr("&File"));

    // File actions
    auto fileopenact = new QAction(tr("&Open..."), this);
    fileopenact->setStatusTip(tr("Open an existing file"));
    fileopenact->setShortcuts(QKeySequence::Open);
    connect(fileopenact, SIGNAL(triggered()), this, SLOT(open()));
    filemenu->addAction(fileopenact);

    filemenu->addSeparator();

    auto fileexitact = new QAction(tr("E&xit"), this);
    fileexitact->setStatusTip(tr("Exit the application"));
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

}
