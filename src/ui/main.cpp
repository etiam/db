/*
 * main.cp
 *
 *  Created on: Mar 25, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <QApplication>

#include "mainwindow.h"
#include "main.h"

namespace Ui
{

Main::Main(int &argc, char *argv[])
{
    // has to get called before creating instance of QApplication
    QApplication::setDesktopSettingsAware(true);
    m_app = std::make_unique<QApplication>(argc, argv);
    m_app->setStyle("GTK+");

    // create a MainWindow
    m_mainWindow = std::make_unique<MainWindow>();
    m_mainWindow->setObjectName("mainwindow");
    m_mainWindow->show();
}

Main::~Main()
{
}

void
Main::initialize()
{
    m_initialized = true;
}

void
Main::run()
{
    if (!m_initialized)
        initialize();

    m_app->exec();
}

}
