/*
 * loaderThread.h
 *
 *  Created on: Aug 30, 2018
 *      Author: jasonr
 */

#ifndef SRC_GDB_LOADERTHREAD_H_
#define SRC_GDB_LOADERTHREAD_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "core/workerThread.h"

namespace Gdb
{

class LoaderThread: public Core::WorkerThread
{
public:
    LoaderThread();
    virtual ~LoaderThread();

private:
    virtual void process() final;
};

} // namespace Gdb

#endif // SRC_GDB_LOADERTHREAD_H_
