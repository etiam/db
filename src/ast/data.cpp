/*
 * Data.cpp
 *
 *  Created on: Aug 15, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <iostream>
#include <fstream>
#include <sstream>

#include "core/archive.h"
#include "core/serializers.h"

#include "data.h"

namespace Ast
{

bool
Data::save(const std::string &filename)
{
    std::stringstream stream;
    Core::OArchive ar(stream);

    ar << m_filenames;
//    ar << m_functions;

    // append attribute data
    std::fstream file;
    file.open(filename, std::fstream::out | std::fstream::binary);
    file << stream.rdbuf();
    file.close();

    return true;
}

bool
Data::load(const std::string &filename)
{
    return true;
}

} // namespace Ast
