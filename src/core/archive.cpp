/*
 * archive.cpp
 *
 *  Created on: Jan 13, 2015
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <iostream>

#include "archive.h"

namespace Core
{

Archive::Archive(bool isread) :
    m_isread(isread)
{
}

Archive::~Archive()
{
}

void
Archive::read(char* str, int num)
{
    throw;
}

void
Archive::write(const char* str, int num)
{
    throw;
}

bool
Archive::isRead() const
{
    return m_isread;
}

bool
Archive::isWrite() const
{
    return !m_isread;
}


/////////////////////////////////////////////


IArchive::IArchive(std::istream &is) :
    Archive(true),
    m_stream(is)
{
}

void
IArchive::read(char* str, int num)
{
    m_stream.read(str, num);
}


/////////////////////////////////////////////


OArchive::OArchive(std::ostream &os) :
    Archive(false),
    m_stream(os)
{
}

void
OArchive::write(const char* str, int num)
{
    m_stream.write(str, num);
}

} // namespace Core
