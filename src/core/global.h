/*
 * global.h
 *
 *  Created on: Oct 8, 2012
 *      Author: jasonr
 */

#ifndef CORE_GLOBAL_H_
#define CORE_GLOBAL_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "gdb/classPointers.h"
#include "ast/classPointers.h"

#include "classPointers.h"

namespace Core
{

// global init
void                        initialize();
void                        shutdown();

// Access to Managers
StatePtr &                  state();
OptionsManagerPtr &         optionsManager();

Gdb::GdbControllerPtr &     gdbController();
Ast::AstBuilderPtr &        astBuilder();

} // namespace Core

#endif // CORE_GLOBAL_H_
