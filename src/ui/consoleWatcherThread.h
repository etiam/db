/*
 * redirectThread.h
 *
 *  Created on: Sep 2, 2018
 *      Author: jasonr
 */

#ifndef UI_CONSOLEWATCHERTHREAD_H_
#define UI_CONSOLEWATCHERTHREAD_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "core/workerThread.h"

namespace Ui
{

class ConsoleWatcherThread: public Core::WorkerThread
{
public:
    ConsoleWatcherThread(int stdout, int stderr);
    virtual ~ConsoleWatcherThread();

protected:
    // custom version of run that doesn't wait for a trigger
    virtual void run() final;

private:
    virtual void process() final;
    void readAndSignal(int fd);

    int m_stdout, m_stderr;
};

} // namespace Ui

#endif // UI_CONSOLEWATCHERTHREAD_H_
