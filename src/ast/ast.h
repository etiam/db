/*
 * ast.h
 *
 *  Created on: Mar 23, 2018
 *      Author: jasonr
 */

#ifndef AST_AST_H_
#define AST_AST_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <memory>

class AstImpl;

class Ast
{
  public:
    Ast();
    ~Ast();

    void parseFile(const std::string &filename);

  private:
    std::unique_ptr<AstImpl>    m_impl;
};

#endif // AST_AST_H_
