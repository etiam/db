/*
 * variables.h
 *
 *  Created on: Oct 23, 2018
 *      Author: jasonr
 */

#ifndef SRC_CORE_VARIABLES_H_
#define SRC_CORE_VARIABLES_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string>
#include <boost/any.hpp>

namespace Core
{



class Variables
{
public:
    Variables();
    ~Variables() = default;

    using Entries = std::vector<Variable>;

    Entries & entries();

private:
    int m_currentFrame = 0;
    Entries m_entries;
};

} // namespace Core

#endif // SRC_CORE_VARIABLES_H_
