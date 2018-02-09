#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMap>
#include <QMainWindow>
#include "editor.h"

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QString &filename, QWidget *parent=0);
    ~MainWindow();

    void readSettings();
    void writeSettings();

private:
    Ui::MainWindow *    ui;
    Editor *            editor;
};

#endif // MAINWINDOW_H
