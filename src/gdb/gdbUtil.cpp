/*
 * gdbUtiil.cpp
 *
 *  Created on: Apr 10, 2018
 *      Author: jasonr
 */


#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <regex>

#include "core/global.h"

#include "gdbController.h"
#include "gdbResult.h"

namespace
{

static std::regex addrRegex(R"regex(Symbol \\"(.*)\(.*\)\\" is a function at addrRegex (0x[0-9a-f]+)\.\\n)regex");
static std::regex lineRegex(R"regex(Symbol \\"(.*)\(.*\)\\" is a function at addrRegex (0x[0-9a-f]+)\.\\n)regex");

bool addressFilter(const Gdb::GdbResult &result, int token);
void addressResponse(const Gdb::GdbResult &result, int token);
bool lineFilter(const Gdb::GdbResult &result, int token);
void lineResponse(const Gdb::GdbResult &result, int token);

bool
addressFilter(const Gdb::GdbResult &result, int token)
{
    return (result.token.value == -1 &&
            result.message.type == Gdb::Message::Type::NONE &&
            result.payload.type == Gdb::Payload::Type::STRING &&
            result.stream == Gdb::Stream::STDOUT &&
            std::regex_match(result.payload.string.string, addrRegex));
};

void
addressResponse(const Gdb::GdbResult &result, int token)
{
    std::smatch match;
    if (std::regex_match(result.payload.string.string, match, addrRegex))
    {
        std::string cmd = "interpreter-exec console \"info line *" + match[2].str() + "\"";
        Core::gdbController()->executeCommand(cmd, lineFilter, lineResponse);
    }
}

bool
lineFilter(const Gdb::GdbResult &result, int token)
{
    return (result.token.value == -1 &&
            result.message.type == Gdb::Message::Type::NONE &&
            result.payload.type == Gdb::Payload::Type::STRING &&
            std::regex_match(result.payload.string.string, lineRegex));
}

void
lineResponse(const Gdb::GdbResult &result, int token)
{
    std::smatch match;
    if (std::regex_match(result.payload.string.string, match, lineRegex))
    {
    }
}

}

namespace Gdb
{

namespace Util
{

void jumpToProgramStart()
{
    std::string cmd = "interpreter-exec console \"info address main\"";
    Core::gdbController()->executeCommand(cmd, addressFilter, addressResponse);
}

}

}





