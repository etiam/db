/*
 * commands.cpp
 *
 *  Created on: Apr 21, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "handlers.h"
#include "controller.h"
#include "commands.h"

namespace Gdb
{

Commands::Commands() :
    m_controller(std::make_unique<Controller>())
{
    m_controller->addResponse(Handlers::fileexecresponse);
    m_controller->addResponse(Handlers::breakinsertresponse);
    m_controller->addResponse(Handlers::infoaddressresponse);
    m_controller->addResponse(Handlers::infolineresponse);
}

Commands::~Commands()
{
}

int
Commands::executeCommand(const std::string &command, Controller::ResponseFunc response, bool persistent)
{
    return m_controller->executeCommand(command, response, persistent);
}

void
Commands::loadProgram(const std::string &filename)
{
    std::string cmd = "file-exec-and-symbols " + filename;
    m_controller->executeCommand(cmd);
}

void
Commands::insertBreakpoint(const std::string &location)
{
    std::string cmd = "break-insert  " + location;
    m_controller->executeCommand(cmd);
}

void
Commands::infoAddress(const std::string &function)
{
    std::string cmd = "interpreter-exec console \"info address " + function + "\"";
    m_controller->executeCommand(cmd);

}


} // namespace Gdb
