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
#include <iostream>

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

}

#endif // SRC_CORE_TYPES_H_
