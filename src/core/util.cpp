/*
 * util.c
 *
 *  Created on: May 3, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <sstream>
#include <thread>

#include "libut/utils.h"

#include "global.h"
#include "optionsManager.h"

namespace Core
{

std::string
dumpStack()
{
    std::stringstream stack;

    char threadname[16];
    pthread_getname_np(pthread_self(), threadname, 16);

    stack << "\"" << threadname << "\" " << std::hex << std::showbase << std::this_thread::get_id() << std::dec << std::noshowbase;

    auto full = optionsManager()->has("fullstackdump") &&
                optionsManager()->get<bool>("fullstackdump");
    full = true;
    stack << ", stack dump : " << std::endl << Ut::dumpStack(full);

    return stack.str();
}

}
