/*
 * archive.h
 *
 *  Created on: Jan 13, 2015
 *      Author: jasonr
 */

#pragma once
#ifndef CORE_ARCHIVE_H_
#define CORE_ARCHIVE_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <istream>
#include <ostream>

namespace Core
{

class Archive
{
  public:
    Archive(bool isread);
    virtual ~Archive();

    virtual void            read(char* str, int num);
    virtual void            write(const char* str, int num);

    bool                    isRead() const;
    bool                    isWrite() const;

    const bool              m_isread;
};


/////////////////////////////////////////////


class IArchive : public Archive
{
  public:
    IArchive(std::istream &is);
    ~IArchive() = default;

    virtual void read(char* str, int num);

  private:
    std::istream &   m_stream;
};


/////////////////////////////////////////////


class OArchive : public Archive
{
  public:
    OArchive(std::ostream &os);
    ~OArchive() = default;

    virtual void write(const char* str, int num);

  private:
    std::ostream &   m_stream;
};


/////////////////////////////////////////////


template <typename T>
Archive &
operator <<(Archive &ar, T &t)
{
    serialize(ar, t);
    return ar;
}

template <typename T>
Archive &
operator >>(Archive &ar, T &t)
{
    serialize(ar, t);
    return ar;
}

template <typename T>
Archive &
operator <<(Archive &ar, T *t)
{
    serialize(ar, *t);
    return ar;
}

template <typename T>
Archive &
operator >>(Archive &ar, T *t)
{
    serialize(ar, *t);
    return ar;
}

template <typename T>
Archive &
operator &(Archive &ar, T &t)
{
    serialize(ar, t);
    return ar;
}

template <typename T>
Archive &
operator &(Archive &ar, T *t)
{
    serialize(ar, *t);
    return ar;
}

} // namespace Core

#endif // CORE_ARCHIVE_H_
