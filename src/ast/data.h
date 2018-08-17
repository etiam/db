/*
 * Data.h
 *
 *  Created on: Aug 15, 2018
 *      Author: jasonr
 */

#ifndef AST_DATA_H_
#define AST_DATA_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <memory>
#include <unordered_map>

namespace Ast
{

struct ReferenceLocation
{
    unsigned short id;
    unsigned int offset;
    unsigned short length;

    std::size_t
    hash() const
    {
        // Compute individual hash values for first, second and third (http://stackoverflow.com/a/1646913/126995)
        std::size_t hash = 17;
        hash = hash * 31 + std::hash<short>()(id);
        hash = hash * 31 + std::hash<int>()(offset);
        hash = hash * 31 + std::hash<short>()(length);
        return hash;
    }

    bool
    operator==(const ReferenceLocation &other) const
    {
        return (offset == other.offset && length == other.length && id == other.id);
    }
};

struct ReferenceData
{
    std::string name;
};

using References = std::unordered_map<ReferenceLocation, ReferenceData>;
using Filenames = std::unordered_map<std::string, unsigned short>;

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

namespace Ast
{

class Data
{
  public:
    Data() = default;
    ~Data() = default;

    bool save(const std::string &filename);
    bool load(const std::string &filename);

    References m_functions;
    Filenames m_filenames;
};

} // namespace Ast

#endif // AST_DATA_H_
