/*
 * location.h
 *
 *  Created on: Apr 29, 2018
 *      Author: jasonr
 */

#pragma once
#ifndef CORE_LOCATION_H_
#define CORE_LOCATION_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string>

namespace Core
{

struct Location
{
	std::string     m_filename;
	int             m_line;
};

} // namespace Core

#endif // CORE_LOCATION_H_
