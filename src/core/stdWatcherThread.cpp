/*
 * redirectThread.cpp
 *
 *  Created on: Sep 2, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <iostream>
#include <unistd.h>

#include "utils.h"
#include "signals.h"
#include "stdWatcherThread.h"

namespace Core
{

StdWatcherThread::StdWatcherThread(int stdout, int stderr) :
    WorkerThread("consolewatcher"),
    m_stdout(stdout),
    m_stderr(stderr)
{
    m_thread = std::make_unique<std::thread>(&StdWatcherThread::run, std::ref(*this));
}

StdWatcherThread::~StdWatcherThread()
{
}

void
StdWatcherThread::run()
{
    pthread_setname_np(pthread_self(), m_threadName.c_str());

    (void)m_stderr;


    std::cerr << "watching fileno " << m_stdout << std::endl;

    // loop until end is signaled
    while(!m_done)
    {
        m_doneLock.lock();
        try
        {
            process();
        }

        catch (...)
        {
            std::cerr << dumpStack() << std::endl;
        }
        m_doneLock.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void
StdWatcherThread::process()
{
    fd_set rfds;
    struct timeval tv;

    // setup select fds
    FD_ZERO(&rfds);
    FD_SET(m_stdout, &rfds);
    FD_SET(m_stderr, &rfds);

    // no timeout
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    auto retval = select(std::max(m_stdout, m_stderr) + 1, &rfds, NULL, NULL, &tv);

    if (retval == -1)
    {
        perror("select()");
    }
    else if (retval)
    {
        if (FD_ISSET(m_stdout, &rfds))
        {
//            readAndSignal(m_stdout);
        }
        else if (FD_ISSET(m_stderr, &rfds))
        {
            readAndSignal(m_stderr);
        }
    }
}

void
StdWatcherThread::readAndSignal(int fd)
{
    std::string captured;
    const int bufSize = 1025;
    char buf[bufSize];
    int bytesRead = 0;
    bool fd_blocked(false);

    do
    {
        bytesRead = 0;
        fd_blocked = false;
        bytesRead = read(fd, buf, bufSize-1);
        if (bytesRead > 0)
        {
            buf[bytesRead] = 0;
            captured += buf;
        }
        else if (bytesRead < 0)
        {
            fd_blocked = (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR);
            if (fd_blocked)
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    while(fd_blocked || bytesRead == (bufSize-1));

    Core::Signals::appendStdoutText(captured);
}

} // namespace Core
