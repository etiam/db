/*
 * scannerThread.cpp
 *
 *  Created on: Aug 30, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <iostream>
#include <vector>
#include <future>

#include "core/global.h"
#include "core/state.h"
#include "core/signals.h"
#include "core/timer.h"

#include "scanner.h"
#include "scannerThread.h"

namespace Ast
{

ScannerThread::ScannerThread() :
    Core::WorkerThread("astscanner")
{
    m_thread = std::make_unique<std::thread>(&ScannerThread::run, std::ref(*this));
}

ScannerThread::~ScannerThread()
{
}

void
ScannerThread::process()
{
    auto &vars = Core::state()->vars();

    // wait for number of threads to be read by qsettings
    while(!vars.has("numastthreads"));

    auto scannerthreads = vars.get<int>("numastthreads");

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

} // namespace Ast
