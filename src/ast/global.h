/*
 * global.h
 *
 *  Created on: Aug 19, 2018
 *      Author: jasonr
 */

#ifndef AST_GLOBAL_H_
#define AST_GLOBAL_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "classPointers.h"

namespace Ast
{

// global init
void initialize();
void shutdown();

} // namespace Ast

#endif // AST_GLOBAL_H_
