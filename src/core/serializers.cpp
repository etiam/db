/*
 * serializers.cpp
 *
 *  Created on: Jan 7, 2015
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "ast/data.h"

#include "serializers.h"
#include "archive.h"

namespace Core
{

// bool
void
serialize(Archive &ar, bool &t)
{
    if (ar.isRead())
        ar.read((char*)&t, sizeof(t));
    else
        ar.write((char*)&t, sizeof(t));
}

// int
void
serialize(Archive &ar, int &t)
{
    if (ar.isRead())
        ar.read((char*)&t, sizeof(t));
    else
        ar.write((char*)&t, sizeof(t));
}

// unsigned int
void
serialize(Archive &ar, unsigned int &t)
{
    if (ar.isRead())
        ar.read((char*)&t, sizeof(t));
    else
        ar.write((char*)&t, sizeof(t));
}

// unsigned short
void
serialize(Archive &ar, unsigned short &t)
{
    if (ar.isRead())
        ar.read((char*)&t, sizeof(t));
    else
        ar.write((char*)&t, sizeof(t));
}

// ulong &
void
serialize(Archive &ar, unsigned long &t)
{
    if (ar.isRead())
        ar.read((char*)&t, sizeof(t));
    else
        ar.write((char*)&t, sizeof(t));
}

// ulong
void
serialize(Archive &ar, const unsigned long t)
{
    if (ar.isRead())
        throw;
    else
        ar.write((char*)&t, sizeof(t));
}

// float
void
serialize(Archive &ar, float &t)
{
    if (ar.isRead())
        ar.read((char*)&t, sizeof(t));
    else
        ar.write((char*)&t, sizeof(t));
}

// std::string
void
serialize(Archive &ar, std::string &t)
{
    if (ar.isRead())
    {
        unsigned short size;
        ar.read((char*)&size, sizeof(size));
        t.resize(size);
        ar.read(&t[0], size);
    }
    else
    {
        unsigned short size = t.size();
        ar.write((char*)&size, sizeof(size));
        ar.write((char*)&t[0], size * sizeof(char));
    }
}

void
serialize(Archive &ar, Ast::ReferenceLocation &t)
{
    if (ar.isRead())
    {
        ar >> t.id;
        ar >> t.length;
        ar >> t.offset;
    }
    else
    {
        ar << t.id;
        ar << t.length;
        ar << t.offset;
    }
}

void
serialize(Archive &ar, Ast::ReferenceData &t)
{
    if (ar.isRead())
    {
        ar >> t.name;
    }
    else
    {
        ar << t.name;
    }
}

}
