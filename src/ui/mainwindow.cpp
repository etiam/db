#include <iostream>
#include <QtCore>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(const QString &filename, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("db " + filename);

    editor = new Editor(this);

    ui->mainLayout->insertWidget(1, editor, 1);

    readSettings();

    if (!filename.isEmpty())
    {
        QFile file(filename);
        QTextStream stream(&file);

        file.open(QFile::ReadOnly | QFile::Text);
        auto text = stream.readAll();
        editor->setText(text);

        auto numlines = text.count("\n");
        auto numdigits = numlines > 0 ? (int) log10 ((double) numlines) + 1 : 1;
        editor->setGutterWidth(numdigits - 1);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    delete editor;

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
