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
#include <vector>
#include <unordered_map>

namespace Ast
{

struct ReferenceLocation
{
    unsigned int line, colstart, colend;

    std::size_t
    hash() const
    {
        // Compute individual hash values for first, second and third (http://stackoverflow.com/a/1646913/126995)
        std::size_t hash = 17;
        hash = hash * 31 + std::hash<int>()(line);
        hash = hash * 31 + std::hash<int>()(colstart);
        hash = hash * 31 + std::hash<int>()(colend);
        return hash;
    }

    bool
    operator==(const ReferenceLocation &other) const
    {
        return (line == other.line && colstart == other.colstart && colend == other.colend);
    }
};

struct ReferenceData
{
    unsigned int offset;
    std::string spelling;
    std::string filename;
};

using References = std::unordered_map<ReferenceLocation, ReferenceData>;

class BuilderImpl;

class Builder
{
  public:
    Builder();
    ~Builder();

    const std::string & buildPath() const;
    void setBuildPath(const std::string &path);

    void parseFunctions(const std::string &filename);

    const References & functions() const;

  private:
    std::string m_buildPath;
    std::unique_ptr<BuilderImpl> m_impl;
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

#endif // AST_BUILDER_H_
