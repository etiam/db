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

namespace Gdb
{

namespace Handlers
{

void fileExec(const std::string &filename);
void breakInsert(const std::string &function);
void infoAddress(const std::string &function);

}

}

#endif // GDB_UTIL_H_
