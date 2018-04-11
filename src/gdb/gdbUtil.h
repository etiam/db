/*
 * gdbUtil.h
 *
 *  Created on: Apr 10, 2018
 *      Author: jasonr
 */


#pragma once
#ifndef GDB_GDBUTIL_H_
#define GDB_GDBUTIL_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

namespace Gdb
{

namespace Util
{

void jumpToProgramStart();

}

}

#endif // GDB_GDBUTIL_H_
