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

#include <memory>
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

    void        readSettings();
    void        writeSettings();

    // menu creation
    void        createMenus();
    void        createFileMenu();

    void        createHotkeys();

  private Q_SLOTS:
    void        loadFile(const QString &filename);
    void        loadFileComplete();
    void        setCursorPosition(int row, int column);
    void        appendConsoleText(const QString &text, bool newline);

  private:
    // wink signal handlers
    void        onLoadFileSignal(const std::string &filename);
    void        onLoadFileCompleteSignal();
    void        onSetCursorPositionSignal(int row, int column);
    void        onAppendConsoleTextSignal(const std::string &text, bool newline);

    Editor *        m_editor;
    Console *       m_console;
    QTabWidget *    m_tabWidget;
    QSplitter *     m_splitter;
};

}

#endif // UI_MAINWINDOW_H_
