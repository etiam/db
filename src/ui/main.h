/*
 * main.h
 *
 *  Created on: Apr 5, 2012
 *      Author: jasonr
 */

#ifndef UI_MAIN_H_
#define UI_MAIN_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <memory>

class QApplication;
class MainWindow;

namespace Core
{
class Redirector;
}

namespace Ui
{

class ConsoleWatcherThread;

class Main
{
public:
    Main(int &argc, char *argv[]);
    ~Main();

    void run();

private:
    void initialize();

    void readSettings();
    void writeSettings() const;

    bool m_initialized = false;

    std::unique_ptr<QApplication> m_app;
    std::unique_ptr<MainWindow> m_mainWindow;
    std::unique_ptr<Core::Redirector> m_stdout;
    std::unique_ptr<Core::Redirector> m_stderr;
    std::unique_ptr<ConsoleWatcherThread> m_consoleWatcher;
};

}

#endif // UI_MAIN_H_
