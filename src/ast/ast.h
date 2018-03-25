/*
 * ast.h
 *
 *  Created on: Mar 23, 2018
 *      Author: jasonr
 */

#ifndef AST_H_
#define AST_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <memory>
#include <clang-c/Index.h>

class Ast
{
  public:
    Ast();
    ~Ast();

    void parseFile(const std::string &filename);

  private:
    CXIndex             m_index;
};

#endif // AST_H_
