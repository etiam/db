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
#include <boost/filesystem/operations.hpp>

#include "core/global.h"
#include "core/signal.h"
#include "core/timer.h"
#include "core/state.h"
#include "core/optionsManager.h"

#include "ast/builder.h"
#include "gdb/commands.h"
#include "ui/main.h"

using namespace boost::program_options;

void
startupThread(const variables_map &vm)
{
    pthread_setname_np(pthread_self(), "startup");

    auto &state = Core::state();
    auto &gdb = Core::gdb();
    auto &ast = Core::astBuilder();
    auto &vars = state->vars();

    // load prog
    if (vm.count("prog"))
    {
        auto filename = vm["prog"].as<std::string>();
        auto buildpath = boost::filesystem::absolute(boost::filesystem::path(filename).parent_path()).string();

        vars.set("filename", filename);
        vars.set("buildpath", buildpath);

        Core::Signal::appendConsoleText("Reading symbols from " + filename + "...");
        gdb->loadProgram(filename);

        if (Core::optionsManager()->get<bool>("breakonmain"))
            gdb->insertBreakpoint("main");
        else
            gdb->infoAddress("main");

//        gdb->executeCommand("file-list-exec-source-files");

        ast->setBuildPath(buildpath);
    }

    else
    {
        vars.set("filename", std::string());
        vars.set("buildpath", std::string());
    }
}

int
main(int argc, char *argv[])
{
    Core::Timer timer;
    pthread_setname_np(pthread_self(), "main");

    // Declare a group of options that will be on command line
    options_description generic("Command line options");
    generic.add_options()
        ("version", "print version string.")
        ("help,h", "produce help message.");

#ifdef DEBUG_ENABLED
    generic.add_options()
    ("verbose,v", "be noisy.");
#endif

    // Hidden options, will be allowed both on command line and
    // in config file, but will not be shown to the user.
    options_description hidden("Hidden options");
    hidden.add_options()
        ("prog", value<std::string>(), "prog");

    options_description cmdline_options;
    cmdline_options.add(generic);
    cmdline_options.add(hidden);

    positional_options_description p;
    p.add("prog", -1);

    variables_map vm;
    store(command_line_parser(argc, argv).options(cmdline_options).positional(p).run(), vm);
    notify(vm);

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
        Core::optionsManager()->set("verbose", true);
    }

    auto gui = std::make_unique<Ui::Main>(argc, argv);
    auto startupthread = std::make_unique<std::thread>(&startupThread, vm);

    std::cout << "startup in " << timer << std::endl;
    gui->run();

    startupthread->join();
}
