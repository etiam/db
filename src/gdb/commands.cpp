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
#include "core/signals.h"

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
Commands::executeConsoleCommand(const std::string &command, Controller::HandlerFunc handler, boost::any data)
{
    m_controller->executeCommand("interpreter-exec console \"" + command + "\"", handler, data);
}

void
Commands::loadProgram(const std::string &filename)
{
    m_controller->executeCommand("file-exec-and-symbols " + filename, Handlers::fileexec);
}

void
Commands::setArgs(const std::string &args)
{
    m_controller->executeCommand("exec-arguments " + args);
}

void
Commands::getSourceFiles()
{
    m_controller->executeCommand("file-list-exec-source-files", Handlers::listsourcefiles);
}

void
Commands::getFunctionNames()
{
    executeConsoleCommand("info functions", Handlers::infofunctions);
}

void
Commands::updateCallStack()
{
    m_controller->executeCommand("stack-list-frames", Handlers::stacklistframes);
}

void
Commands::infoAddress(const std::string &function)
{
    executeConsoleCommand("info address " + function);
}

void
Commands::insertBreakpoint(const std::string &location)
{
    std::string cmd = "break-insert " + location;
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
    Core::Signals::appendConsoleText("run\n");
    m_controller->executeCommand(cmd, Handlers::execrun);
}

void
Commands::cont()
{
    std::string cmd = "exec-continue";
    Core::Signals::appendConsoleText("continue\n");
    m_controller->executeCommand(cmd, Handlers::execrun);
}

void
Commands::pause()
{
    // send SIGINT to prog
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
        executeConsoleCommand("kill", Handlers::killprog);
    }
}

void
Commands::stepover()
{
    Core::Signals::appendConsoleText("next\n");
    m_controller->executeCommand("exec-next", Handlers::execnext);
}

void
Commands::stepinto()
{
    Core::Signals::appendConsoleText("step\n");
    m_controller->executeCommand("exec-step");
}

void
Commands::stepout()
{
    Core::Signals::appendConsoleText("finish\n");
    m_controller->executeCommand("exec-finish");
}

} // namespace Gdb
