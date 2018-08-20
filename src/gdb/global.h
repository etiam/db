/*
 * global.h
 *
 *  Created on: Aug 19, 2018
 *      Author: jasonr
 */

#ifndef GDB_GLOBAL_H_
#define GDB_GLOBAL_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "classPointers.h"

namespace Gdb
{

// global init
void initialize();
void shutdown();

} // namespace Gdb

#endif // GDB_GLOBAL_H_
