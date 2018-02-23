#include <iostream>
#include <fstream>
#include <sstream>
#include <QtCore>
#include <QApplication>

#include "gdb/pyGdbMiInterface.h"
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    PyGdbMiInterface gdb;

    MainWindow w(argv[1]);
    w.show();

    return app.exec();
}
