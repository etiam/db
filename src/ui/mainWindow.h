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

namespace Ui
{

class Editor;
class Console;
class DebugControls;

class MainWindow : public QMainWindow
{
  Q_OBJECT

  public:
    MainWindow(QWidget *parent=0);
    ~MainWindow();

  private Q_SLOTS:
    void            quit();

    void            run();
    void            pause();
    void            stop();
    void            reload();
    void            stepover();
    void            stepinto();
    void            stepout();

    void            closeTab(int index);
    void            switchTab(int index);

    void            toggleConsoleTab();
    void            toggleOutputTab();
    void            toggleGdbTab();

  private:
    void            readSettings();
    void            writeSettings() const;

    void            createToolbar();
    void            createDocks();
    void            createMenus();
    void            createFileMenu();
    void            createViewMenu();

    void            createHotkeys();

    // wink signal handlers
    void            onLoadFileSignal(const std::string &filename);
    void            onAppendConsoleText(const std::string &text);
    void            onAppendLogText(const std::string &text);
    void            onAppendOutputText(const std::string &text);
    void            onDebuggerStateSet(Core::State::Debugger state);

    Editor *        m_editor;
    QTabWidget *    m_tabWidget;

    Console *       m_consoleTab;
    Console *       m_gdbTab;
    Console *       m_outputTab;

    DebugControls * m_debugControls;
};

}

#endif // UI_MAINWINDOW_H_
