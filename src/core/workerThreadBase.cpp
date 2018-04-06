/*
 * workerThread.cpp
 *
 *  Created on: Feb 25, 2015
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <iostream>

#include "workerThreadBase.h"

WorkerThreadBase::WorkerThreadBase(const std::string &name) :
    m_name(name)
{
}

WorkerThreadBase::~WorkerThreadBase()
{
    m_done = true;
    if (m_thread)
        m_thread->join();
}

void
WorkerThreadBase::run()
{
    pthread_setname_np(pthread_self(), m_name.c_str());

    try
    {
        if (!m_initialized)
        {
            initialize();
            m_initialized = true;
        }
    }

    catch (const std::exception &e)
    {
        std::cerr << "look: uncaught std::exception: " << e.what() << std::endl;
    }

    catch (...)
    {
        std::cerr << "look: uncaught unknown exception." << std::endl;
    }

    // loop until end is signaled
    while(!m_done)
    {
        try
        {
            process();
        }

        catch (const std::system_error &e)
        {
            std::cerr << "look: system error: " << e.code().message() << std::endl;
        }

        catch (const std::exception &e)
        {
            std::cerr << "look: uncaught std::exception: " << e.what() << std::endl;
        }

        catch (...)
        {
            std::cerr << "look: uncaught unknown exception." << std::endl;
        }
    }
}
