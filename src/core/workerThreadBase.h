/*
 * workerThread.h
 *
 *  Created on: Feb 25, 2015
 *      Author: jasonr
 */

#pragma once
#ifndef WORKERTHREADBASE_H_
#define WORKERTHREADBASE_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string>
#include <memory>
#include <thread>
#include <mutex>

class WorkerThreadBase
{
  public:
    explicit WorkerThreadBase(const std::string &threadname);
    virtual ~WorkerThreadBase();

  private:
    virtual void                    process()=0;

  protected:
    virtual void                    run();
    virtual void                    initialize() {};

    bool                            m_initialized = false;
    bool                            m_done = false;

    std::string                     m_name;
    std::unique_ptr<std::thread>    m_thread;
};

#endif // WORKERTHREADBASE_H_
