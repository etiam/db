/*
 * workerThread.cpp
 *
 *  Created on: Feb 25, 2015
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <iostream>

#include "utils.h"
#include "workerThread.h"

namespace Core
{

WorkerThread::WorkerThread(const std::string &threadname) :
    m_done(false),
    m_triggered(false),
    m_shouldProcess(false),
    m_threadName(threadname)
{
}

WorkerThread::~WorkerThread()
{
    stop();
    if (m_thread)
        m_thread->join();
}

void
WorkerThread::trigger()
{
    m_shouldProcess = true;
    notify();
}

void
WorkerThread::stop()
{
    // wait for current process to finish before continuing
    {
    m_doneLock.lock();
//    std::unique_lock<std::mutex> locker(m_doneLock);
    m_done = true;
    m_doneLock.unlock();
    }

    m_shouldProcess = false;
    notify();
}

void
WorkerThread::notify()
{
    m_triggered = true;
    std::unique_lock<std::mutex> locker(m_conditionLock);
    m_condition.notify_all();
}

void
WorkerThread::run()
{
    pthread_setname_np(pthread_self(), m_threadName.c_str());

    // loop until end is signaled
    while(!m_done)
    {
        // wait for trigger conditional
        {
        std::unique_lock<std::mutex> locker(m_conditionLock);
        m_condition.wait(locker, [&](){ return m_triggered; });
        }
        m_triggered = false;

        if (m_shouldProcess)
        {
            m_doneLock.lock();
//            std::unique_lock<std::mutex> locker(m_doneLock);
            try
            {
                process();
            }

            catch (...)
            {
                std::cerr << "db: uncaught unknown exception." << std::endl;
                std::cerr << dumpStack() << std::endl;
            }
            m_doneLock.unlock();
        }
    }
}

} // namespace Core
