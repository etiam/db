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

class ConsoleWatcher: public WorkerThread
{
public:
    ConsoleWatcher();
    virtual ~ConsoleWatcher();

private:
    virtual void process() final;
};

} // namespace Core

#endif // CORE_REDIRECTTHREAD_H_
