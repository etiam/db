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
class Variables;

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    bool eventFilter(QObject *object, QEvent *event);

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

protected:
    void showEvent(QShowEvent* event);

private Q_SLOTS:
    void quit();
    void onLoadEditorSource(const QString &filename);
    void onDebuggerStateUpdated();

private:
    Editor * m_editor;

    DebugControls * m_debugControls;

    TabWidget * m_bottomTabWidget;

    // tab widgets
    GdbConsole * m_gdbConsoleTab;
    Output * m_programOutputTab;
    ColoredOutput * m_consoleOutputTab;
    CallStack * m_callStackTab;
    BreakPoints * m_breakPointsTab;
    Variables * m_variablesTab;

    QLabel * m_statusIcon;

    QSettings * m_settings;
};

}

#endif // UI_MAINWINDOW_H_
