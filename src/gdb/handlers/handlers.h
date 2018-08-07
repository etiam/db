/*
 * util.h
 *
 *  Created on: Apr 10, 2018
 *      Author: jasonr
 */


#pragma once
#ifndef GDB_HANDLERS_H_
#define GDB_HANDLERS_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <boost/any.hpp>

#include "gdb/result.h"

namespace Gdb
{

namespace Handlers
{

// execution has stopped due to a breakpoint, signal, etc
bool stopped(const Result &result, int token, boost::any data);

// handle stack-list-frames response
bool stacklistframes(const Result &result, int token, boost::any data);

bool threadgroupstarted(const Result &result, int token, boost::any data);

// handle result of info line console command
bool infoline(const Result &result, int token, boost::any data);

// handle result of info address console command
bool infoaddress(const Result &result, int token, boost::any data);

// handle result of info functions console command
bool infofunctions(const Result &result, int token, boost::any data);

// handle interpreter-exec console commands
bool interpreterexec(const Result &result, int token, boost::any data);

bool processexited(const Result &result, int token, boost::any data);
bool killprog(const Result &result, int token, boost::any data);

bool console(const Result &result, int token, boost::any data);
bool logging(const Result &result, int token, boost::any data);
bool notify(const Result &result, int token, boost::any data);
bool output(const Result &result, int token, boost::any data);

bool execrun(const Result &result, int token, boost::any data);
bool execnext(const Result &result, int token, boost::any data);

bool fileexec(const Result &result, int token, boost::any data);
bool listsourcefiles(const Result &result, int token, boost::any data);

bool breakinsert(const Result &result, int token, boost::any data);
bool breakdelete(const Result &result, int token, boost::any data);
bool breakenable(const Result &result, int token, boost::any data);
bool breakdisable(const Result &result, int token, boost::any data);

// when a breakpoint changes, hitcount, enabled, etc.
bool breakmodified(const Result &result, int token, boost::any data);

}

}

#endif // GDB_HANDLERS_H_
