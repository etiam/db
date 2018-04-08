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

#include <string>
#include <memory>

namespace Ast
{

class AstBuilderImpl;

class AstBuilder
{
  public:
    AstBuilder();
    ~AstBuilder();

    void setBuildPath(const std::string &path);
    void parseFile(const std::string &filename);

  private:
    std::unique_ptr<AstBuilderImpl>    m_impl;
};

}

#endif // AST_AST_H_
