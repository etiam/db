#include <QtCore>
#include <QApplication>

#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow w(argv[1]);
    w.show();

    return app.exec();
}
