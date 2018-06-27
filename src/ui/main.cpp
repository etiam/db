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
#include <QFile>
#include <QSettings>

#include "core/global.h"
#include "core/optionsManager.h"

#include "mainWindow.h"
#include "main.h"

namespace Ui
{

Main::Main(int &argc, char *argv[])
{
    // has to get called before creating instance of QApplication
    QApplication::setDesktopSettingsAware(true);
    m_app = std::make_unique<QApplication>(argc, argv);
    m_app->setStyle("GTK+");

    // for qsettings
    m_app->setOrganizationName("jasonrosson");
    m_app->setOrganizationDomain("jasonrosson.com");
    m_app->setApplicationName("db");

    // apply global stylesheet
    QFile stylefile(":/qss/custom.qss");
    stylefile.open(QFile::ReadOnly);
    QString style(stylefile.readAll());
    m_app->setStyleSheet(style);

    readSettings();

    // create a MainWindow
    m_mainWindow = std::make_unique<MainWindow>();
    m_mainWindow->setObjectName("mainwindow");
    m_mainWindow->show();
}

Main::~Main()
{
    writeSettings();
}

void
Main::run()
{
    if (!m_initialized)
        initialize();

    m_app->exec();
}

void
Main::initialize()
{
    m_initialized = true;
}

void
Main::readSettings()
{
    QSettings settings;

    Core::optionsManager()->set("breakonmain", settings.value("Core/BreakOnMain", true).toBool());
}

void
Main::writeSettings() const
{
    QSettings settings;

    settings.setValue("Core/BreakOnMain", Core::optionsManager()->get<bool>("breakonmain"));
}

}
