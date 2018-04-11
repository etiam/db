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

namespace Ui
{

class Main
{
  public:
    Main(int &argc, char *argv[]);
    ~Main();

    void                            run();

  private:
    void                            initialize();

    bool                            m_initialized = false;

    std::unique_ptr<QApplication>   m_app;
    std::unique_ptr<MainWindow>     m_mainWindow;
};

}

#endif // UI_MAIN_H_
