/*
 * gdbUtiil.cpp
 *
 *  Created on: Apr 10, 2018
 *      Author: jasonr
 */


#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <iostream>
#include <regex>
#include <boost/filesystem/operations.hpp>

#include "core/global.h"
#include "core/state.h"
#include "core/signal.h"

#include "controller.h"
#include "result.h"

namespace
{

bool
loadfileresponse(const Gdb::Result &result, int token)
{
    auto ret = result.token.value == token;

    if (ret)
    {
        Core::loadFileCompleteSignal();
    }

    return ret;
}

}

namespace Gdb
{

namespace Util
{

void loadFile(const std::string &filename)
{
    std::string cmd = "file-exec-and-symbols " + filename;
    Core::gdb()->executeCommand(cmd, loadfileresponse);

}

}

}
