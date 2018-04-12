/*
 * optionsManager.h
 *
 *  Created on: Oct 13, 2013
 *      Author: jasonr
 */

#pragma once
#ifndef CORE_OPTIONSMANAGER_H_
#define CORE_OPTIONSMANAGER_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string>
#include <sstream>

#include <boost/any.hpp>

namespace Core
{

class OptionsManager
{
  public:
    OptionsManager() = default;
    ~OptionsManager() = default;

    bool                                    hasOption(const std::string &option) const
    {
        return (m_optionsMap.find(option) != std::end(m_optionsMap));
    }

    template <typename T>
    T                                       getOption(const std::string &option) const;

    template <typename T>
    void                                    setOption(const std::string &option, const T &value);

  private:
    std::map<std::string, boost::any>       m_optionsMap;
};

template <typename T>
T
OptionsManager::getOption(const std::string &name) const
{
    T retval;

    if (m_optionsMap.find(name) == m_optionsMap.end())
    {
        std::stringstream errmsg;
        errmsg << "Core::OptionsManager::getOption() : option \"" << name << "\" not found.";
        throw std::runtime_error(errmsg.str().c_str());
    }

    try
    {
        retval = boost::any_cast<T>(m_optionsMap.at(name));
    }
    catch (boost::bad_any_cast &)
    {
        std::stringstream errmsg;
        errmsg << "Core::OptionsManager::getOption() : option \"" << name << "\" can't be cast to the requested type.";
        throw std::runtime_error(errmsg.str().c_str());
    }

    return retval;
}

template <typename T>
void
OptionsManager::setOption(const std::string &name, const T &value)
{
    m_optionsMap[name] = value;
}

} // namespace Core

#endif // CORE_OPTIONSMANAGER_H_
