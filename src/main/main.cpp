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
#include <future>
#include <boost/program_options.hpp>
#include <boost/filesystem/operations.hpp>

#include "core/util.h"
#include "core/global.h"
#include "core/signals.h"
#include "core/timer.h"
#include "core/state.h"
#include "core/optionsManager.h"
#include "ast/scanner.h"
#include "gdb/commands.h"
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
gdbStartupThread(const po::variables_map &vm)
{
    pthread_setname_np(pthread_self(), "gdbstartup");

    // try to find prog in current dir
    if (vm.count("prog"))
    {
        auto &state = Core::state();
        auto &gdb = Core::gdb();
        auto &vars = state->vars();

        bool found = false;
        auto prog = vm["prog"].as<std::string>();
        if(!boost::filesystem::exists(vm["prog"].as<std::string>()))
        {

            // otherwise search $PATH
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

            vars.set("filename", prog);
            vars.set("buildpath", buildpath);

            // will output to console
            gdb->executeCommand("gdb-version");

            // give gdb-version time to complete
            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            Core::Signals::appendConsoleText("Reading symbols from " + prog + "...");
            gdb->loadProgram(prog);

            // set program arguments
            if (vm.count("args"))
            {
                const auto &args = vm["args"].as<std::vector<std::string>>();
                std::string argstr = std::accumulate(std::begin(args), std::end(args), std::string{},
                    [](std::string &s, const std::string &piece) -> decltype(auto) { return s += piece + " "; });
                gdb->setArgs(argstr);
            }

            // if breakonmain true, set breakpoint, otherwise find source file for main
            if (Core::optionsManager()->get<bool>("breakonmain"))
                gdb->insertBreakpoint("main");
            else
                gdb->infoAddress("main");

            gdb->getSourceFiles();
        }

        else
        {
            std::stringstream msg;
            msg << vm["prog"].as<std::string>() << " : No such file or directory." << std::endl;
            Core::Signals::appendConsoleText(msg.str());

            vars.set("filename", std::string());
            vars.set("buildpath", std::string());
        }
    }
}

void
astStartupThread(const po::variables_map &vm)
{
    pthread_setname_np(pthread_self(), "aststartup");

    bool signaled = false;
    Core::Signals::sourceListUpdated.connect([&signaled]() { signaled = true; });

    // wait for sourceListUpdated signal to fire
    while(!signaled)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // TODO : get this from gsettings
    auto scannerthreads = 8;

    auto buildpath = Core::state()->vars().get<std::string>("buildpath");
    const auto &sourcefiles = Core::state()->sourceFiles();
    const auto count = sourcefiles.size();

    // number of jobs per thread
    auto binsize = count / scannerthreads == 0 ? count : count / scannerthreads;

    std::vector<std::shared_future<Ast::Scanner>> jobs;

    // scanner lambda, scans sourcefiles between indices start and end
    auto loadem = [&](int start, int end)
    {
        Ast::Scanner localast;
        localast.setBuildPath(buildpath);

        for (auto n=start; n < end; ++ n)
        {
            auto filename = sourcefiles[n];

            Core::Signals::setStatusbarText("parsing " + filename);
            localast.parseFunctions(filename);
        }

        return localast;
    };

    Core::Timer timer;

    // launch scanner async jobs
    int start = 0;
    for (auto n=0; n < scannerthreads; ++n)
    {
        if (start >= count)
            break;

        // make sure last job in last thread is last source file
        auto end = start+binsize+1 > count ? count-1 : start+binsize;

        jobs.push_back(std::async(std::launch::async, loadem, start, end));

        start += binsize+1;
    }

    // combine results of async jobs
    auto &functions = Core::state()->functions();
    std::for_each(std::begin(jobs), std::end(jobs), [&](std::shared_future<Ast::Scanner> j)
    {
        const auto &f = j.get().functions();
        functions.insert(std::begin(f), std::end(f));
    });

    std::cout << "scanned " << count << " files in " << timer << " ms" << std::endl;

    Core::Signals::setStatusbarText("");
    Core::Signals::functionListUpdated();
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
        Core::optionsManager()->set("verbose", true);
    }

    // start gui
    auto gui = std::make_unique<Ui::Main>(argc, argv);

    // start startup thread
    auto gdbstartupthread = std::make_unique<std::thread>(&gdbStartupThread, vm);
    auto aststartupthread = std::make_unique<std::thread>(&astStartupThread, vm);

    std::cout << "startup in " << timer << " ms" << std::endl;
    gui->run();

    gdbstartupthread->join();
    aststartupthread->join();
}
