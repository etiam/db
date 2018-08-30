/*
 * scannerThread.h
 *
 *  Created on: Aug 30, 2018
 *      Author: jasonr
 */

#ifndef SRC_AST_SCANNERTHREAD_H_
#define SRC_AST_SCANNERTHREAD_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "core/workerThread.h"

namespace Ast
{

class ScannerThread: public Core::WorkerThread
{
public:
    ScannerThread();
    virtual ~ScannerThread();

private:
    virtual void process() final;
};

} // namespace Ast

#endif // SRC_AST_SCANNERTHREAD_H_
