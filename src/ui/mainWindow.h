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
class TabWidget;
class Output;
class ColoredOutput;
class GdbConsole;
class CallStack;
class DebugControls;
class BreakPoints;

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    // toggle the visibility of tab
    void toggleTab(QWidget *tab);

    // called when tab close is requested
    void closeBottomTab(int index);

    // called when tab is switched
    void switchBottomTab(int index);

    // save/restore window settings
    void readSettings();
    void writeSettings() const;

    // create various gui elements
    void createToolbar();
    void createDocks();
    void createMenus();
    void createFileMenu();
    void createDebugMenu();
    void createViewMenu();
    void createStatusbar();

    void createHotkeys();

    // signal handlers
    void onLoadFileSignal(const std::string &filename);
    void onDebuggerStateUpdated();

protected:
    void showEvent(QShowEvent* event);

private Q_SLOTS:
    void quit();

private:
    Editor * m_editor;

    TabWidget * m_bottomTabWidget;

    GdbConsole * m_gdbConsoleTab;
    Output * m_programOutputTab;
    ColoredOutput * m_consoleOutputTab;
    CallStack * m_callStackTab;
    BreakPoints * m_breakPointsTab;

    DebugControls * m_debugControls;

    QLabel * m_statusIcon;

    QSettings * m_settings;
};

}

#endif // UI_MAINWINDOW_H_
