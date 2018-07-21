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

#include "core/state.h"

class QTabWidget;
class QSplitter;
class QLabel;

namespace Ui
{

class Editor;
class Output;
class Console;
class CallStack;
class DebugControls;

class MainWindow : public QMainWindow
{
  Q_OBJECT

  public:
    MainWindow(QWidget *parent=0);
    ~MainWindow();

  public Q_SLOTS:

    void            run();
    void            pause();
    void            stop();
    void            stepover();
    void            stepinto();
    void            stepout();

  private Q_SLOTS:
    void            quit();

    // toggle the visibility of tab
    void            toggleTab(QWidget *tab);

    // called when tab close is requested
    void            closeTab(int index);

    // called when tab is switched
    void            switchTab(int index);

  private:
    void            readSettings();
    void            writeSettings() const;

    void            createToolbar();
    void            createDocks();
    void            createMenus();
    void            createFileMenu();
    void            createViewMenu();

    void            createHotkeys();

    void            createStatusbar();

    // wink signal handlers
    void            onLoadFileSignal(const std::string &filename);
    void            onAppendConsoleText(const std::string &text);
    void            onAppendLogText(const std::string &text);
    void            onAppendOutputText(const std::string &text);

    void            onDebuggerStateUpdated();

    Editor *        m_editor;
    QTabWidget *    m_tabWidget;

    Console *       m_consoleTab;
    Output *        m_debuggerOutputTab;
    Output *        m_programOutputTab;
    CallStack *     m_callStackTab;

    DebugControls * m_debugControls;

    QLabel *        m_statusIcon;
};

}

#endif // UI_MAINWINDOW_H_
