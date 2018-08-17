/*
 * scanner.h
 *
 *  Created on: Mar 23, 2018
 *      Author: jasonr
 */

#ifndef AST_SCANNER_H_
#define AST_SCANNER_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string>

namespace Ast
{

class Data;
class ScannerImpl;

class Scanner
{
  public:
    Scanner();
    ~Scanner();

    void setBuildPath(const std::string &path);

    void parseFunctions(const std::string &filename);

    const Data & data() const;

  private:
    std::shared_ptr<ScannerImpl> m_impl;
};

}

#endif // AST_SCANNER_H_
