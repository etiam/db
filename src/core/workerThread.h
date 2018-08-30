/*
 * workerThread.h
 *
 *  Created on: Feb 25, 2015
 *      Author: jasonr
 */

#pragma once
#ifndef CORE_WORKERTHREAD_H_
#define CORE_WORKERTHREAD_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace Core
{

class WorkerThread
{
public:
    WorkerThread(const std::string &threadname);
    virtual ~WorkerThread();

    virtual void trigger();
    virtual void stop();

private:
    void notify();
    virtual void process() = 0;

protected:
    virtual void run();

    bool m_done;
    bool m_triggered;
    bool m_shouldProcess;

    std::string m_threadName;
    std::unique_ptr<std::thread> m_thread;

    std::mutex m_doneLock;
    std::mutex m_conditionLock;
    std::condition_variable m_condition;
};

}

#endif // CORE_WORKERTHREAD_H_
