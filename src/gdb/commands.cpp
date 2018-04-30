/*
 * commands.cpp
 *
 *  Created on: Apr 21, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "handlers/handlers.h"
#include "controller.h"
#include "commands.h"

namespace Gdb
{

Commands::Commands() :
    m_controller(std::make_unique<Controller>())
{
    m_controller->addHandler(Handlers::stopped, 10, true);

    m_controller->addHandler(Handlers::console, 99, true);
    m_controller->addHandler(Handlers::logging, 99, true);
    m_controller->addHandler(Handlers::output, 99, true);
}

Commands::~Commands()
{
}

int
Commands::executeCommand(const std::string &command, Controller::HandlerFunc handler, boost::any data)
{
    return m_controller->executeCommand(command, handler, data);
}

void
Commands::loadProgram(const std::string &filename)
{
    std::string cmd = "file-exec-and-symbols " + filename;
    m_controller->executeCommand(cmd, Handlers::fileexec);
}

void
Commands::infoAddress(const std::string &function)
{
    std::string cmd = "interpreter-exec console \"info address " + function + "\"";
    m_controller->executeCommand(cmd);

}

void
Commands::insertBreakpoint(const std::string &location)
{
    std::string cmd = "break-insert  " + location;
    m_controller->executeCommand(cmd, Handlers::breakinsert);
}

void
Commands::disableBreakpoint(int number)
{
    std::string cmd = "break-disable " + std::to_string(number);
    m_controller->executeCommand(cmd, Handlers::breakdisable, number);
}

void
Commands::deleteBreakpoint(int number)
{
    std::string cmd = "break-delete " + std::to_string(number);
    m_controller->executeCommand(cmd, Handlers::breakdelete, number);
}

void
Commands::run()
{
    std::string cmd = "exec-run";
    m_controller->executeCommand(cmd);
}

void
Commands::pause()
{
    std::string cmd = "exec-interrupt --all";
    m_controller->executeCommand(cmd);
}

void
Commands::stepover()
{
    std::string cmd = "exec-next";
    m_controller->executeCommand(cmd);
}

void
Commands::stepinto()
{
    std::string cmd = "exec-step";
    m_controller->executeCommand(cmd);
}

void
Commands::stepout()
{
    std::string cmd = "exec-finish";
    m_controller->executeCommand(cmd);
}

} // namespace Gdb