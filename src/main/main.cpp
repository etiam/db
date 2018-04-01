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
#include <boost/program_options.hpp>

#include "ast/ast.h"
#include "gdb/pyGdbMiInterface.h"
#include "ui/main.h"

int main(int argc, char *argv[])
{
    using namespace boost::program_options;

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
        std::cout << "Usage: db [options] [input file]" << std::endl << std::endl;
        std::cout << generic << std::endl;
        std::exit(0);
    }

    // --version
    if (vm.count("version"))
    {
        std::cout << "version " << PACKAGE_VERSION << std::endl;
        std::exit(0);
    }

    // load prog
    if (vm.count("prog"))
    {
        auto filename = vm["prog"].as<std::string>();

        auto ast = std::make_unique<Ast>(filename);
        auto gdb = std::make_unique<PyGdbMiInterface>(filename);

//        ast->parseFile(filename);
        gdb->executeCommand("-file-exec-and-symbols " + filename);
    }

    auto main = std::make_unique<Ui::Main>(argc, argv);
    main->run();
}
