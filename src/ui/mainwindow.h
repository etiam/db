/*
 * mainWindow.h
 *
 *  Created on: Feb 6, 2018
 *      Author: jasonr
 */

#ifndef UI_MAINWINDOW_H_
#define UI_MAINWINDOW_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <QMap>
#include <QMainWindow>

class QTabWidget;
class QSplitter;

namespace Ui
{

class Editor;
class Console;

class MainWindow : public QMainWindow
{
  Q_OBJECT

  public:
    MainWindow(QWidget *parent=0);
    ~MainWindow();

  private Q_SLOTS:
    void            run();
    void            pause();
    void            stepover();
    void            stepinto();
    void            stepout();

  private:
    void            readSettings();
    void            writeSettings() const;

    void            createToolbar();
    void            createMenus();
    void            createFileMenu();

    void            createHotkeys();

    Editor *        m_editor;
    Console *       m_console;
    QTabWidget *    m_tabWidget;
    QSplitter *     m_splitter;
};

}

#endif // UI_MAINWINDOW_H_
