#include <iostream>
#include "core/signal.h"

#include <QFile>
#include <QTextStream>
#include <QWidget>
#include <QVBoxLayout>
#include <QSettings>
#include <QMenu>
#include <QMenuBar>
#include <QSplitter>

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
    auto splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Vertical);
    splitter->setObjectName("splitter");
    splitter->setContentsMargins(0, 0, 0, 0);
    setCentralWidget(splitter);

    // main editor window
    m_editor = new Editor();

    // tabs window
    m_tabWidget = new QTabWidget();

    // tabs within tab window
    m_console = new Console();
    m_tabWidget->addTab(m_console, tr("Console"));

    // add editor and tabwidget to main
    splitter->addWidget(m_editor);
    splitter->addWidget(m_tabWidget);

    // set splitter sizes
    auto h = size().height();
    splitter->setSizes(QList<int>{ static_cast<int>(h*0.85), static_cast<int>(h*0.15) });

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
