#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMap>
#include <QMainWindow>

#include "ast/ast.h"
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
    Ui::MainWindow *    m_ui;
    Editor *            m_editor;
    Ast                 m_ast;
};

#endif // MAINWINDOW_H
