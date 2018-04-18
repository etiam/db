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

#include "gdb/controller.h"
#include "gdb/result.h"

namespace
{

bool
fileexecresponse(const Gdb::Result &result, int token)
{
    auto ret = result.token.value == token;

    if (ret)
        Core::appendConsoleTextSignal(result.message.string.string, true);

    return ret;
}

}

namespace Gdb
{

namespace Handlers
{

void fileExec(const std::string &filename)
{
    std::string cmd = "file-exec-and-symbols " + filename;
    Core::gdb()->executeCommand(cmd, fileexecresponse);
}

}

}
