/*
 * redirectThread.cpp
 *
 *  Created on: Sep 2, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <iostream>

#include "utils.h"
#include "stdWatcherThread.h"

namespace Core
{

StdWatcherThread::StdWatcherThread(int stdout, int stderr) :
    WorkerThread("consolewatcher"),
    m_stdout(stdout),
    m_stderr(stderr)
{
    m_thread = std::make_unique<std::thread>(&StdWatcherThread::run, std::ref(*this));
}

StdWatcherThread::~StdWatcherThread()
{
}

void
StdWatcherThread::run()
{
    pthread_setname_np(pthread_self(), m_threadName.c_str());

    (void)m_stderr;

    // setup select fds
    FD_ZERO(&m_rfds);
    FD_SET(m_stdout, &m_rfds);

    // no timeout
    m_tv.tv_sec = 0;
    m_tv.tv_usec = 0;

    std::cerr << "watching fileno " << m_stdout << std::endl;

    // loop until end is signaled
    while(!m_done)
    {
        m_doneLock.lock();
        try
        {
            process();
        }

        catch (...)
        {
            std::cerr << dumpStack() << std::endl;
        }
        m_doneLock.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

}

void
StdWatcherThread::process()
{
    auto retval = select(m_stdout + 1, &m_rfds, NULL, NULL, &m_tv);

    if (retval == -1)
        perror("select()");
    else if (retval)
        std::cerr << "Data is available now" << std::endl;
//    else
//        std::cerr << "No data" << std::endl;
}

} // namespace Core
