/*
 * util.h
 *
 *  Created on: Apr 10, 2018
 *      Author: jasonr
 */


#pragma once
#ifndef GDB_UTIL_H_
#define GDB_UTIL_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "gdb/result.h"

namespace Gdb
{

namespace Handlers
{

bool fileexecresponse(const Result &result, int token);
bool breakinsertresponse(const Result &result, int token);
bool breakdisableresponse(const Result &result, int token);
bool breakdeleteresponse(const Result &result, int token);
bool infoaddressresponse(const Result &result, int token);
bool infolineresponse(const Result &result, int token);

}

}

#endif // GDB_UTIL_H_
