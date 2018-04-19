/*
 * anymap.h
 *
 *  Created on: Apr 19, 2018
 *      Author: jasonr
 */

#pragma once
#ifndef CORE_ANYMAP_H_
#define CORE_ANYMAP_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string>
#include <sstream>

#include <boost/any.hpp>

namespace Core
{

class AnyMap
{
  public:
    AnyMap() = default;
    ~AnyMap() = default;

    bool                                    has(const std::string &key) const;

    template <typename T>
    T                                       get(const std::string &key) const;

    template <typename T>
    void                                    set(const std::string &key, const T &value);

  private:
    std::map<std::string, boost::any>       m_map;
};

inline
bool
AnyMap::has(const std::string &key) const
{
    return (m_map.find(key) != std::end(m_map));
}

template <typename T>
T
AnyMap::get(const std::string &name) const
{
    T retval;

    if (m_map.find(name) == m_map.end())
    {
        std::stringstream errmsg;
        errmsg << "Core::AnyMap::get() : key \"" << name << "\" not found.";
        throw std::runtime_error(errmsg.str().c_str());
    }

    try
    {
        retval = boost::any_cast<T>(m_map.at(name));
    }
    catch (boost::bad_any_cast &)
    {
        std::stringstream errmsg;
        errmsg << "Core::AnyMap::get() : key \"" << name << "\" can't be cast to the requested type.";
        throw std::runtime_error(errmsg.str().c_str());
    }

    return retval;
}

template <typename T>
void
AnyMap::set(const std::string &name, const T &value)
{
    m_map[name] = value;
}

} // namespace Core

#endif // CORE_ANYMAP_H_
