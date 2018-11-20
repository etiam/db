/*
 * types.h
 *
 *  Created on: Jul 23, 2018
 *      Author: jasonr
 */

#ifndef SRC_CORE_TYPES_H_
#define SRC_CORE_TYPES_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string>
#include <map>
#include <iostream>

#include <boost/any.hpp>

namespace Core
{

struct Location
{
    std::string     function;
    std::string     filename;
    int             row;

    bool operator ==(const Location &other) const
    {
        return other.function == function && other.filename == filename && other.row == row;
    }
};

inline
std::ostream &
operator <<(std::ostream &out, const Location &location)
{
    return out << "(" << location.function << ", " << location.filename << ", " << location.row << ")";
}

struct Variable
{
    Variable() : type(""), value(0) {}
    Variable(const std::string &type_, const boost::any &value_) : type(type_), value(value_) {}
    std::string type;
    boost::any value;    // TODO : switch this to std::string
};

using Variables = std::map<std::string, Variable>;

struct CallStackEntry
{
    CallStackEntry(Location loc, int le) :
        location(std::move(loc)),
        level(le) {}
    Location location;
    int level;
};

using CallStack = std::vector<CallStackEntry>;

}

#endif // SRC_CORE_TYPES_H_
