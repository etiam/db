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
#include <QStandardPaths>

#include "core/util.h"
#include "core/global.h"
#include "core/signals.h"
#include "core/timer.h"
#include "core/state.h"

#include "gdb/global.h"
#include "gdb/commands.h"
#include "ast/global.h"
#include "ui/main.h"

namespace po = boost::program_options;

// https://stackoverflow.com/questions/11295019/environment-path-directories-iteration
const std::vector<std::string> &
get_environment_path()
{
    static std::vector<std::string> result;
    if (!result.empty())
        return result;

#if _WIN32
    const std::string path = convert_to_utf8( _wgetenv(L"PATH") ); // Handle Unicode, just remove if you don't want/need this. convert_to_utf8 uses WideCharToMultiByte in the Win32 API
    const char delimiter = ';';
#else
    const std::string path = getenv("PATH");
    const char delimiter = ':';
#endif
    if (path.empty())
        throw std::runtime_error("PATH should not be empty");

    size_t previous = 0;
    size_t index = path.find(delimiter);
    while (index != std::string::npos)
    {
        result.push_back(path.substr(previous, index - previous));
        previous = index + 1;
        index = path.find(delimiter, previous);
    }
    result.push_back(path.substr(previous));

    return result;
}

void
processArgs(const po::variables_map &vm)
{
    // try to find prog in current dir
    if (vm.count("prog"))
    {
        auto &state = Core::state();
        auto &gdb = Gdb::commands();
        auto &vars = state->vars();

        bool found = false;
        auto prog = vm["prog"].as<std::string>();

        // if prog doesn't exist in current dir search $PATH for it
        if(!boost::filesystem::exists(vm["prog"].as<std::string>()))
        {
            auto path = get_environment_path();
            for (const auto &p : path)
            {
                if (boost::filesystem::exists(boost::filesystem::path(p) / prog))
                {
                    prog = (boost::filesystem::path(p) / prog).string();
                    found = true;
                    break;
                }
            }
        }
        else
        {
            found = true;
        }

        // load prog
        if (found)
        {
            auto prog = vm["prog"].as<std::string>();
            auto buildpath = boost::filesystem::canonical(boost::filesystem::path(prog).parent_path()).string();

            vars.set("progname", prog);
            vars.set("buildpath", buildpath);

            // give gdb-version time to complete
            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            Core::Signals::appendConsoleText("Reading symbols from " + prog + "...");
            gdb->loadProgram(prog);
        }
        else
        {
            std::stringstream msg;
            msg << vm["prog"].as<std::string>() << " : No such file or directory." << std::endl;
            Core::Signals::appendConsoleText(msg.str());

            vars.set("progname", std::string());
            vars.set("buildpath", std::string());
        }
    }
}

int
main(int argc, char *argv[])
{
    Core::Timer timer;
    pthread_setname_np(pthread_self(), "main");

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

    // initialization
    Core::initialize();
    Gdb::initialize();
    Ast::initialize();

    // process command line args
    processArgs(vm);

    // start gui
    auto gui = std::make_unique<Ui::Main>(argc, argv);

    std::cout << "startup in " << timer << " ms" << std::endl;
    gui->run();
}
