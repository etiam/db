/*
 * global.cpp
 *
 *  Created on: Aug 19, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <iostream>
#include <memory>
#include <thread>
#include <future>

#include <boost/utility.hpp>

#include "core/global.h"
#include "core/state.h"
#include "core/signals.h"
#include "core/timer.h"

#include "global.h"
#include "scanner.h"

namespace Ast
{

class Master: boost::noncopyable
{
public:
    ~Master();

    static void initialize();
    static void shutdown();

private:
    Master();

    static Master & instance();

    void WorkerThread();

    std::unique_ptr<std::thread> m_workerThread;
};

std::unique_ptr<Master> g_instance;

void
Master::initialize()
{
    instance();
}

void
Master::shutdown()
{
    if (g_instance)
        g_instance.reset();
}

Master::Master() :
    m_workerThread(std::make_unique<std::thread>(&Master::WorkerThread, std::ref(*this)))
{
}

Master::~Master()
{
    m_workerThread->join();
}

Master &
Master::instance()
{
    if (!g_instance)
        g_instance = std::unique_ptr<Master>(new Master());
    return *g_instance;
}


void
Master::WorkerThread()
{
    pthread_setname_np(pthread_self(), "astworker");

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
    auto scanfiles = [&](int start, int end)
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

        jobs.push_back(std::async(std::launch::async, scanfiles, start, end));

        start += binsize+1;
    }

    // combine results of async jobs
    auto &astdata = Core::state()->astData();
    auto &functions = astdata.m_functions;
    auto &filenames = astdata.m_filenames;
    std::for_each(std::begin(jobs), std::end(jobs), [&](std::shared_future<Ast::Scanner> job)
    {
        const auto &func = job.get().data().m_functions;
        functions.insert(std::begin(func), std::end(func));

        const auto &filename = job.get().data().m_filenames;
        filenames.insert(std::begin(filename), std::end(filename));
    });

    std::cout << "scanned " << count << " files in " << timer << " ms" << std::endl;

    Core::Signals::setStatusbarText("");
    Core::Signals::functionListUpdated();

    // store ast in cache
    astdata.save("/home/jasonr/ast.data");

//    auto res = QStandardPaths::standardLocations(QStandardPaths::CacheLocation);
//    auto res = QStandardPaths::standardLocations(QStandardPaths::ConfigLocation);
//    for (const auto &path : res)
//        std::cout << path.toStdString() << std::endl;

}

////////////////////

void
initialize()
{
    Master::initialize();
}

void
shutdown()
{
    Master::shutdown();
}

} // namespace Ast
