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
#include "core/state.h"
#include "core/redirector.h"

#include "consoleWatcherThread.h"
#include "mainWindow.h"
#include "main.h"

namespace Ui
{

Main::Main(int &argc, char *argv[])
{
    // setup stdout/stderr redirectors
    if (!Core::state()->vars().has("nostdcap"))
    {
        m_stdout = std::make_unique<Core::Redirector>(stdout);
        m_stderr = std::make_unique<Core::Redirector>(stderr);

        if (m_stdout->getReadDescriptor() != -1 && m_stderr->getReadDescriptor() != -1)
        {
            m_consoleWatcher = std::make_unique<ConsoleWatcherThread>(m_stdout->getReadDescriptor(),
                                                                      m_stderr->getReadDescriptor());
        }
    }

    // has to get called before creating instance of QApplication
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication::setDesktopSettingsAware(true);
    m_app = std::make_unique<QApplication>(argc, argv);
    m_app->setStyle("GTK+");

    // for qsettings
    m_app->setApplicationName("db");

    // apply global stylesheet
    QFile stylefile(":/qss/custom.qss");
    stylefile.open(QFile::ReadOnly);
    QString style(stylefile.readAll());
    m_app->setStyleSheet(style);

    readSettings();

    // create a MainWindow
    m_mainWindow = std::make_unique<MainWindow>();
    m_mainWindow->show();

    // capture all events to enable global hotkeys
    m_app->installEventFilter(m_mainWindow.get());
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

    auto &vars = Core::state()->vars();

    vars.set("breakonmain", settings.value("Core/BreakOnMain", true).toBool());
    vars.set("numastthreads", settings.value("Core/AstReaderThreads", 4).toInt());

    /*
    unsigned int n = std::thread::hardware_concurrency();
    */
}

void
Main::writeSettings() const
{
    QSettings settings;

    const auto &vars = Core::state()->vars();
    settings.setValue("Core/BreakOnMain", vars.get<bool>("breakonmain"));
    settings.setValue("Core/AstReaderThreads", vars.get<int>("numastthreads"));
}

}
