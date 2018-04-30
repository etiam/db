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

bool console(const Result &result, int token, boost::any data);
bool logging(const Result &result, int token, boost::any data);
bool output(const Result &result, int token, boost::any data);

bool stopped(const Result &result, int token, boost::any data);

bool fileexec(const Result &result, int token, boost::any data);
bool breakinsert(const Result &result, int token, boost::any data);
bool breakdisable(const Result &result, int token, boost::any data);
bool breakdelete(const Result &result, int token, boost::any data);
bool infoaddress(const Result &result, int token, boost::any data);
bool infoline(const Result &result, int token, boost::any data);

}

}

#endif // GDB_HANDLERS_H_