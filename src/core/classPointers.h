/*
 * classPointers.h
 *
 *  Created on: Apr 11, 2018
 *      Author: jasonr
 */

#ifndef CORE_CLASSPOINTERS_H_
#define CORE_CLASSPOINTERS_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

namespace Core
{

class State;
class OptionsManager;

using StatePtr                  = std::unique_ptr<State>;
using OptionsManagerPtr         = std::unique_ptr<OptionsManager>;

}

#endif // CORE_CLASSPOINTERS_H_
