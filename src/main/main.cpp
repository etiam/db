#include <iostream>
#include <fstream>
#include <sstream>
#include <QtCore>
#include <QApplication>

#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    /*
    {
    std::ifstream t(argv[1]);
    std::stringstream buffer;
    buffer << t.rdbuf();
    auto str = buffer.str();

    for (auto c : str)
        std::cout << std::hex << (int)c << " ";
    std::cout << std::endl;
    }

    {
    QFile file(argv[1]);
    QTextStream stream(&file);

    file.open(QFile::ReadOnly | QFile::Text);
    auto text = stream.readAll();
    auto str = text.toStdString();

    for (auto c : str)
        std::cout << std::hex << (int)c << " ";
    std::cout << std::endl;

    auto newstr = text.replace(R"(\n)", R"(\\n)").toStdString();

    for (auto c : newstr)
        std::cout << std::hex << (int)c << " ";
    std::cout << std::endl;
    }
    */

//    exit(0);

    MainWindow w(argv[1]);
    w.show();

    return app.exec();
}
