/*
 * redirector.h
 *
 *  Created on: Sep 4, 2018
 *      Author: jasonr
 */

#ifndef SRC_CORE_REDIRECTOR_H_
#define SRC_CORE_REDIRECTOR_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

namespace Core
{

class Redirector
{
public:
    Redirector(FILE *fp);
    ~Redirector();

    enum PIPES
    {
        READ,
        WRITE
    };

    int getReadDescriptor() const;

private:
    bool m_redirected = false;
    int m_fn;
    int m_oldfd;
    int m_pipefd[2];
};

} // namespace Core

#endif // SRC_CORE_REDIRECTOR_H_
