/*
 * builder.h
 *
 *  Created on: Mar 23, 2018
 *      Author: jasonr
 */

#ifndef AST_BUILDER_H_
#define AST_BUILDER_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string>
#include <memory>

namespace Ast
{

class BuilderImpl;

class Builder
{
  public:
    Builder();
    ~Builder();

    const std::string & buildPath() const;
    void setBuildPath(const std::string &path);

    void parseFile(const std::string &filename);

  private:
    std::string m_buildPath;
    std::unique_ptr<BuilderImpl> m_impl;
};

}

#endif // AST_BUILDER_H_
