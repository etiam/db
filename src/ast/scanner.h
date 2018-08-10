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
#include <memory>
#include <vector>
#include <unordered_map>

namespace Ast
{

struct ReferenceLocation
{
    std::string filename;
    unsigned int offset, length;

    std::size_t
    hash() const
    {
        // Compute individual hash values for first, second and third (http://stackoverflow.com/a/1646913/126995)
        std::size_t hash = 17;
        hash = hash * 31 + std::hash<int>()(offset);
        hash = hash * 31 + std::hash<int>()(length);
        hash = hash * 31 + std::hash<std::string>()(filename);
        return hash;
    }

    bool
    operator==(const ReferenceLocation &other) const
    {
        return (offset == other.offset && length == other.length && filename == other.filename);
    }
};

struct ReferenceData
{
    std::string name;
};

using References = std::unordered_map<ReferenceLocation, ReferenceData>;

class ScannerImpl;

class Scanner
{
  public:
    Scanner();
    ~Scanner();

    void setBuildPath(const std::string &path);

    void parseFunctions(const std::string &filename);

    const References & functions() const;

  private:
    std::string m_buildPath;
    std::shared_ptr<ScannerImpl> m_impl;
};

}

namespace std
{

template <>
struct hash<Ast::ReferenceLocation>
{
  public:
    std::size_t
    operator()(const Ast::ReferenceLocation &key) const
    {
        return key.hash();
    }
};

}

#endif // AST_SCANNER_H_
