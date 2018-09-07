/*
 * global.cpp
 *
 *  Created on: Sep 2, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <iostream>
#include <memory>

#include <boost/utility.hpp>

#include <QApplication>
#include <QSettings>
#include <QFile>

#include "core/global.h"
#include "core/state.h"

#include "mainWindow.h"
#include "global.h"

namespace Ui
{

class Application : public QApplication
{
public:
    Application(int &argc, char *argv[]) : QApplication(argc, argv) { std::cout << "Application" << std::endl; };
    ~Application() { std::cout << "~Application" << std::endl; };
};

class Master: boost::noncopyable
{
public:
    ~Master();

    static void initialize(int &argc, char *argv[]);
    static void shutdown();
    static int run();

private:
    Master(int &argc, char *argv[]);

    static Master & instance(int &argc, char *argv[]);

    std::unique_ptr<Application> m_app;
    std::unique_ptr<MainWindow> m_mainWindow;
};

std::unique_ptr<Master> g_instance;

void
Master::initialize(int &argc, char *argv[])
{
    instance(argc, argv);
}

void
Master::shutdown()
{
    if (g_instance)
        g_instance.reset();
}

int
Master::run()
{
    int ret = 0;

    if (g_instance)
    {
        ret = g_instance->m_app->exec();
    }

    return ret;
}

Master::Master(int &argc, char *argv[])
{
    // has to get called before creating instance of QApplication
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication::setDesktopSettingsAware(true);
    m_app = std::make_unique<Application>(argc, argv);
    m_app->setStyle("GTK+");
    m_app->setApplicationName("db");

    // apply global stylesheet
    QFile stylefile(":/qss/custom.qss");
    stylefile.open(QFile::ReadOnly);
    QString style(stylefile.readAll());
    m_app->setStyleSheet(style);

    // create a MainWindow
    m_mainWindow = std::make_unique<MainWindow>();
    m_mainWindow->setObjectName("mainwindow");
    m_mainWindow->show();

    // read core settings
    QSettings settings;
    auto &vars = Core::state()->vars();
    vars.set("breakonmain", settings.value("Core/BreakOnMain", true).toBool());

    std::cout << "Ui::Master" << std::endl;
}

Master::~Master()
{
    std::cout << m_app->applicationName().toStdString() << std::endl;

    // write core settings
    QSettings settings;
    const auto &vars = Core::state()->vars();
    settings.setValue("Core/BreakOnMain", vars.has("breakonmain") && vars.get<bool>("breakonmain"));
}

Master &
Master::instance(int &argc, char *argv[])
{
    if (!g_instance)
        g_instance = std::unique_ptr<Master>(new Master(argc, argv));
    return *g_instance;
}

////////////////////

void
initialize(int &argc, char *argv[])
{
    Master::initialize(argc, argv);
}

void
shutdown()
{
    Master::shutdown();
}

void
run()
{
    Master::run();
}

} // namespace Ui
