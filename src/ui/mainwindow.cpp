#include <iostream>
#include "core/signal.h"

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
    centralwidgetlayout->setSpacing(2);
    centralwidgetlayout->setObjectName("centralwidgetlayout");
    centralwidgetlayout->setContentsMargins(0, 0, 0, 0);

    m_editor = new Editor(this);
    m_console = new Console(this);

    centralwidgetlayout->addWidget(m_editor, 1);
    centralwidgetlayout->addWidget(m_console, 0);
    setCentralWidget(centralwidget);

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
    m_console->appendPlainText("done");
}

void
MainWindow::setCursorPosition(int row, int column)
{
    m_editor->setCursorPosition(row, column);
}

void
MainWindow::appendConsoleText(const QString &text)
{
//    auto prevcursor = m_console->textCursor();
    m_console->insertPlainText(text);
//    m_console->setTextCursor(prevcursor);
    m_console->moveCursor(QTextCursor::Up);
    m_console->moveCursor(QTextCursor::EndOfLine);
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
MainWindow::onAppendConsoleTextSignal(const std::string &text)
{
    QMetaObject::invokeMethod(this, "appendConsoleText", Qt::QueuedConnection, Q_ARG(QString, QString::fromStdString(text)));
}

}
