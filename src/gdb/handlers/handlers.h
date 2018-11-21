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
#include "gdb/controller.h"

namespace Gdb
{

namespace Handlers
{

// execution has stopped due to a breakpoint, signal, etc
Controller::HandlerReturn stopped(const Result &result, int token, boost::any data);

// handle stack-list-variables --simple-values response
Controller::HandlerReturn stacklistvariablessimple(const Result &result, int token, boost::any data);

// handle stack-list-variables --all-values response
Controller::HandlerReturn stacklistvariablesall(const Result &result, int token, boost::any data);

// handle stack-list-frames response
Controller::HandlerReturn stacklistframes(const Result &result, int token, boost::any data);

// handle stack-info-frame response
Controller::HandlerReturn stackinfoframe(const Result &result, int token, boost::any data);

Controller::HandlerReturn threadgroupstarted(const Result &result, int token, boost::any data);

// handle result of info line console command
Controller::HandlerReturn infoline(const Result &result, int token, boost::any data);

// handle result of info address console command
Controller::HandlerReturn infoaddress(const Result &result, int token, boost::any data);

// handle result of info sources console command
Controller::HandlerReturn infosources(const Result &result, int token, boost::any data);

// handle interpreter-exec console commands
Controller::HandlerReturn interpreterexec(const Result &result, int token, boost::any data);

Controller::HandlerReturn processexited(const Result &result, int token, boost::any data);
Controller::HandlerReturn killprog(const Result &result, int token, boost::any data);

Controller::HandlerReturn console(const Result &result, int token, boost::any data);
Controller::HandlerReturn logging(const Result &result, int token, boost::any data);
Controller::HandlerReturn notify(const Result &result, int token, boost::any data);
Controller::HandlerReturn output(const Result &result, int token, boost::any data);

Controller::HandlerReturn running(const Result &result, int token, boost::any data);
Controller::HandlerReturn execnext(const Result &result, int token, boost::any data);

Controller::HandlerReturn fileexec(const Result &result, int token, boost::any data);
Controller::HandlerReturn listsourcefiles(const Result &result, int token, boost::any data);

Controller::HandlerReturn breakinsert(const Result &result, int token, boost::any data);
Controller::HandlerReturn breakdelete(const Result &result, int token, boost::any data);
Controller::HandlerReturn breakenable(const Result &result, int token, boost::any data);
Controller::HandlerReturn breakdisable(const Result &result, int token, boost::any data);

// when a breakpoint changes, hitcount, enabled, etc.
Controller::HandlerReturn breakmodified(const Result &result, int token, boost::any data);

// when a gdb-version returns
Controller::HandlerReturn gdbversion(const Result &result, int token, boost::any data);

}

}

#endif // GDB_HANDLERS_H_
