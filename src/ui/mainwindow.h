#ifndef UI_MAINWINDOW_H_
#define UI_MAINWINDOW_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <memory>
#include <QMap>
#include <QMainWindow>

#include "editor.h"

class Ast;
class PyGdbMiInterface;

namespace Ui
{

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QString &filename, QWidget *parent=0);
    ~MainWindow();

    void readSettings();
    void writeSettings();

private:
    Editor *                m_editor;
    std::unique_ptr<Ast>    m_ast;
};

}

#endif // UI_MAINWINDOW_H_
