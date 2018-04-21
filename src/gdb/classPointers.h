/*
 * classPointers.h
 *
 *  Created on: Apr 7, 2018
 *      Author: jasonr
 */

#pragma once
#ifndef GDB_CLASSPOINTERS_H_
#define GDB_CLASSPOINTERS_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <memory>

namespace Gdb
{

class Commands;

using CommandsPtr         = std::unique_ptr<Commands>;

}

#endif
