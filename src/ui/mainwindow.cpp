#include <iostream>
#include <QFile>
#include <QTextStream>
#include <QWidget>
#include <QHBoxLayout>
#include <QSettings>

#include "mainwindow.h"

namespace Ui
{

MainWindow::MainWindow(const QString &filename, QWidget *parent) :
    QMainWindow(parent)
{
    setWindowTitle("db " + filename);

    // gui setup
    auto centralWidget = new QWidget(this);
    centralWidget->setObjectName(QStringLiteral("centralWidget"));
    centralWidget->setEnabled(true);

    auto horizontalLayout = new QHBoxLayout(centralWidget);
    horizontalLayout->setSpacing(0);
    horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
    horizontalLayout->setContentsMargins(0, 0, 0, 0);

    auto mainLayout = new QHBoxLayout();
    mainLayout->setSpacing(0);
    mainLayout->setObjectName(QStringLiteral("mainLayout"));

    horizontalLayout->addLayout(mainLayout);

    setCentralWidget(centralWidget);

    m_editor = new Editor(this);

    mainLayout->insertWidget(1, m_editor, 1);

    // settings
    readSettings();

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

}
