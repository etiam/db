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
class QSettings;

namespace Ui
{

class Editor;
class Output;
class Console;
class CallStack;
class DebugControls;
class BreakPoints;

class MainWindow : public QMainWindow
{
  Q_OBJECT

  public:
    MainWindow(QWidget *parent=0);
    ~MainWindow();

  private:
    void            quit();

    // toggle the visibility of tab
    void            toggleTab(QWidget *tab);

    // called when tab close is requested
    void            closeBottomTab(int index);

    // called when tab is switched
    void            switchBottomTab(int index);

    // save/restore window settings
    void            readSettings();
    void            writeSettings() const;

    // create various gui elements
    void            createToolbar();
    void            createDocks();
    void            createMenus();
    void            createFileMenu();
    void            createDebugMenu();
    void            createViewMenu();
    void            createStatusbar();

    void            createHotkeys();

    // wink signal handlers
    void            onLoadFileSignal(const std::string &filename);
    void            onQuitRequested();
    void            onAppendConsoleText(const std::string &text);
    void            onAppendLogText(const std::string &text);
    void            onAppendOutputText(const std::string &text);

    void            onDebuggerStateUpdated();

    Editor *        m_editor;

    QTabWidget *    m_bottomTabWidget;

    Console *       m_consoleTab;
    Output *        m_debuggerOutputTab;
    Output *        m_programOutputTab;
    CallStack *     m_callStackTab;
    BreakPoints *   m_breakPointsTab;

    DebugControls * m_debugControls;

    QLabel *        m_statusIcon;

    QSettings *     m_settings;
};

}

#endif // UI_MAINWINDOW_H_
