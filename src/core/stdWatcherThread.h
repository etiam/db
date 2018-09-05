/*
 * redirectThread.h
 *
 *  Created on: Sep 2, 2018
 *      Author: jasonr
 */

#ifndef CORE_REDIRECTTHREAD_H_
#define CORE_REDIRECTTHREAD_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "workerThread.h"

namespace Core
{

class StdWatcherThread: public WorkerThread
{
public:
    StdWatcherThread(int stdout, int stderr);
    virtual ~StdWatcherThread();

protected:
    // custom version of run that doesn't wait for a trigger
    virtual void run() final;

private:
    virtual void process() final;
    void readAndSignal(int fd);

    int m_stdout, m_stderr;
};

} // namespace Core

#endif // CORE_REDIRECTTHREAD_H_
