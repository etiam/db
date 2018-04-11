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
#include "core/timer.h"
#include "ast/astBuilder.h"
#include "gdb/gdbController.h"
#include "ui/main.h"

using namespace boost::program_options;

void
startupThread(const variables_map &vm)
{
    pthread_setname_np(pthread_self(), "startup");

    auto &gdb = Core::gdbController();
    auto &ast = Core::astBuilder();

    // load prog
    if (vm.count("prog"))
    {
        auto filename = vm["prog"].as<std::string>();

        gdb->executeCommand("file-exec-and-symbols " + filename);
        gdb->jumpToProgramStart();

//        gdb->executeCommand("file-list-exec-source-files");
//        gdb->executeCommand("break-insert main");

        auto buildpath = boost::filesystem::absolute(boost::filesystem::path(filename).parent_path()).string();
        ast->setBuildPath(buildpath);
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

    auto startupthread = std::make_unique<std::thread>(&startupThread, vm);

    auto gui = std::make_unique<Ui::Main>(argc, argv);
    std::cout << "startup in " << timer << std::endl;
    gui->run();

    startupthread->join();
}
