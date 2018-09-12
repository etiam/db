/*
 * main.cpp
 *
 *  Created on: Feb 6, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <iostream>
#include <memory>
#include <thread>
#include <regex>
#include <boost/program_options.hpp>

#include <QSettings>
#include <QStandardPaths>

#include "core/utils.h"
#include "core/global.h"
#include "core/signals.h"
#include "core/timer.h"
#include "core/state.h"

#include "gdb/global.h"
#include "gdb/commands.h"
#include "ast/global.h"
#include "ui/global.h"
#include "ui/main.h"

namespace po = boost::program_options;

int
main(int argc, char *argv[])
{
    Core::Timer timer;
    pthread_setname_np(pthread_self(), "db");

    // Declare a group of options that will be on command line
    po::options_description generic("Command line options");
    generic.add_options()
        ("version", "print version string.")
        ("help,h", "produce help message.");

#ifdef DEBUG_ENABLED
    generic.add_options()
    ("verbose,v", "be noisy.");
#endif

    // Hidden options, will be allowed both on command line and
    // in config file, but will not be shown to the user.
    po::options_description hidden("Hidden options");
    hidden.add_options()
        ("prog", po::value<std::string>(), "prog")
        ("args", po::value<std::vector<std::string>>()->multitoken(), "args");

    po::options_description cmdline_options;
    cmdline_options.add(generic);
    cmdline_options.add(hidden);

    po::positional_options_description p;
    p.add("prog", 1);
    p.add("args", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(cmdline_options).positional(p).run(), vm);
    po::notify(vm);

    // --help
    if (vm.count("help"))
    {
        std::cout << "Usage: db [options] [prog]" << std::endl << std::endl;
        std::cout << generic << std::endl;
        std::exit(0);
    }

    // --version
    if (vm.count("version"))
    {
        std::cout << "version " << PACKAGE_VERSION << std::endl;
        std::exit(0);
    }

    // --verbose
    if (vm.count("verbose"))
    {
        Core::state()->vars().set("verbose", true);
    }

    // store args
    if (vm.count("args"))
    {
        Core::state()->vars().set("args", vm["args"].as<std::vector<std::string>>());
    }

    // store progname
    if (vm.count("prog"))
    {
        Core::state()->vars().set("progname", vm["prog"].as<std::string>());
    }

    // start gui
    auto gui = std::make_unique<Ui::Main>(argc, argv);

    std::cout << "startup in " << timer << " ms" << std::endl;
    gui->run();

    gui.reset();

    std::cout << "bye" << std::endl;
}
