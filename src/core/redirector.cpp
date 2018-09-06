/*
 * redirector.cpp
 *
 *  Created on: Sep 4, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include "redirector.h"

namespace Core
{

Redirector::Redirector(FILE *fp)
{
    m_fn = fileno(fp);

    if (pipe(m_pipefd) == -1)
    {
        perror("pipe");
    }
    else
    {
        fprintf(fp, "redirecting fileno %d (%d %d)\n", fileno(fp), m_pipefd[READ], m_pipefd[WRITE]);

        m_oldfd = dup(m_fn);
        if (m_oldfd == -1 && errno != EBADF)
        {
            perror("dup");
        }
        else
        {
            if (dup2(m_pipefd[WRITE], m_fn) == -1)
            {
                perror("dup2");
            }
            else
                m_redirected = true;
        }
    }
}

Redirector::~Redirector()
{
    if (m_redirected)
        dup2(m_oldfd, m_fn);
}


int
Redirector::getReadDescriptor() const
{
    return m_redirected ? m_pipefd[READ] : -1;
}

} // namespace Core
