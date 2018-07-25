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
        auto prog = vm["prog"].as<std::string>();
        auto buildpath = boost::filesystem::absolute(boost::filesystem::path(prog).parent_path()).string();

        vars.set("filename", prog);
        vars.set("buildpath", buildpath);

        Core::Signal::appendConsoleText("Reading symbols from " + prog + "...");
        gdb->loadProgram(prog);

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

    // start gui
    auto gui = std::make_unique<Ui::Main>(argc, argv);

    // make sure prog exists in search path and if so expand the filename out to
    // full pathname
    if (vm.count("prog"))
    {
        auto prog = vm["prog"].as<std::string>();
        if(!boost::filesystem::exists(vm["prog"].as<std::string>()))
        {
            bool found = false;
            auto path = get_environment_path();
            for (const auto &p : path)
            {
                if (boost::filesystem::exists(boost::filesystem::path(p) / prog))
                {
                    auto fullpath = (boost::filesystem::path(p) / prog).string();

                    // replace "prog" with full path
                    // https://groups.google.com/forum/#!topic/boost-list/rmuB9iwtQ34
                    auto it(vm.find("prog"));
                    boost::program_options::variable_value & vx(it->second);
                    vx.value() = fullpath;

                    found = true;
                    break;
                }
            }

            if (!found)
            {
                std::stringstream msg;
                msg << vm["prog"].as<std::string>() << " : No such file or directory." << std::endl;
                Core::Signal::appendConsoleText(msg.str());
                vm.erase(vm.find("prog"));
            }
        }
    }

    // start startup thread
    auto startupthread = std::make_unique<std::thread>(&startupThread, vm);

    std::cout << "startup in " << timer << " ms" << std::endl;
    gui->run();

    startupthread->join();
}
