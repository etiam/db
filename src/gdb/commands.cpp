/*
 * commands.cpp
 *
 *  Created on: Apr 21, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <sys/types.h>
#include <signal.h>

#include <iostream>

#include "core/global.h"
#include "core/state.h"
#include "core/signal.h"

#include "handlers/handlers.h"
#include "controller.h"
#include "commands.h"

namespace Gdb
{

Commands::Commands() :
    m_controller(std::make_unique<Controller>())
{
    // add persistent handlers, lower number priority tested first
    m_controller->addHandler(Handlers::stopped,             10, true);
    m_controller->addHandler(Handlers::threadgroupstarted,  10, true);
    m_controller->addHandler(Handlers::infoline,            10, true);
    m_controller->addHandler(Handlers::infoaddress,         10, true);
    m_controller->addHandler(Handlers::interpreterexec,     10, true);
    m_controller->addHandler(Handlers::processexited,       10, true);

    m_controller->addHandler(Handlers::breakinsert,         10, true);
    m_controller->addHandler(Handlers::breakdelete,         10, true);
    m_controller->addHandler(Handlers::breakmodified,       10, true);

    m_controller->addHandler(Handlers::console,             99, true);
    m_controller->addHandler(Handlers::logging,             99, true);
    m_controller->addHandler(Handlers::notify,              99, true);
    m_controller->addHandler(Handlers::output,              99, true);
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
Commands::execConsoleCommand(const std::string &command)
{
    std::string cmd = "interpreter-exec console \"" + command + "\"";
    m_controller->executeCommand(cmd, Handlers::interpreterexec);
}

void
Commands::loadProgram(const std::string &filename)
{
    std::string cmd = "file-exec-and-symbols " + filename;
    m_controller->executeCommand(cmd, Handlers::fileexec);
}

void
Commands::setArgs(const std::string &args)
{
    std::string cmd = "exec-arguments " + args;
    m_controller->executeCommand(cmd);
}

void
Commands::listSourceFiles()
{
    m_controller->executeCommand("file-list-exec-source-files", Handlers::listsourcefiles);
}

void
Commands::infoAddress(const std::string &function)
{
    execConsoleCommand("info address " + function);
}

void
Commands::insertBreakpoint(const std::string &location)
{
    std::string cmd = "break-insert  " + location;
    m_controller->executeCommand(cmd, Handlers::breakinsert);
}

void
Commands::deleteBreakpoint(int number)
{
    std::string cmd = "break-delete " + std::to_string(number);
    m_controller->executeCommand(cmd, Handlers::breakdelete, number);
}

void
Commands::enableBreakpoint(int number)
{
    std::string cmd = "break-enable " + std::to_string(number);
    m_controller->executeCommand(cmd, Handlers::breakenable, number);
}

void
Commands::disableBreakpoint(int number)
{
    std::string cmd = "break-disable " + std::to_string(number);
    m_controller->executeCommand(cmd, Handlers::breakdisable, number);
}

void
Commands::run()
{
    std::string cmd = "exec-run";
    Core::Signal::appendConsoleText("run\n");
    m_controller->executeCommand(cmd, Handlers::execrun);
}

void
Commands::cont()
{
    std::string cmd = "exec-continue";
    Core::Signal::appendConsoleText("continue\n");
    m_controller->executeCommand(cmd, Handlers::execrun);
}

void
Commands::pause()
{
    if(Core::state()->vars().has("pid"))
    {
        auto pid = Core::state()->vars().get<int>("pid");
        kill(pid, SIGINT);
    }
}

void
Commands::stop()
{
    pause();

    if (Core::state()->debuggerState() == Core::State::Debugger::RUNNING ||
        Core::state()->debuggerState() == Core::State::Debugger::PAUSED)
    {
        std::string cmd = "interpreter-exec console \"kill\"";
        m_controller->executeCommand(cmd, Handlers::killprog);
    }
}

void
Commands::stepover()
{
    std::string cmd = "exec-next";
    Core::Signal::appendConsoleText("next\n");
    m_controller->executeCommand(cmd, Handlers::execnext);
}

void
Commands::stepinto()
{
    std::string cmd = "exec-step";
    Core::Signal::appendConsoleText("step\n");
    m_controller->executeCommand(cmd);
}

void
Commands::stepout()
{
    std::string cmd = "exec-finish";
    m_controller->executeCommand(cmd);
}

} // namespace Gdb
