/*
 * classPointers.h
 *
 *  Created on: Apr 7, 2018
 *      Author: jasonr
 */

#pragma once
#ifndef AST_CLASSPOINTERS_H_
#define AST_CLASSPOINTERS_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <memory>

namespace Ast
{

class Builder;

using BuilderPtr               = std::unique_ptr<Builder>;

}

#endif
