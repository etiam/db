/*
 * state.h
 *
 *  Created on: Apr 12, 2018
 *      Author: jasonr
 */


#pragma once
#ifndef CORE_STATE_H_
#define CORE_STATE_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <boost/program_options.hpp>

namespace Core
{

class State
{
  public:
    boost::program_options::variables_map   m_options;
};

}

#endif // CORE_STATE_H_
