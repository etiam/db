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

#include "fmt/format.h"
#include "handlers/handlers.h"

#include "controller.h"
#include "commands.h"

namespace Gdb
{

Commands::Commands() :
    m_controller(std::make_unique<Controller>())
{
    // add persistent handlers, lower number priority tested first
    m_controller->addHandler(Handlers::running,             10, true);
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
    Core::state()->callStack().clear();

    // get all stack frame data
    m_controller->executeCommand("stack-list-frames", Handlers::stacklistframes);

    // get the current stack frame
    m_controller->executeCommand("stack-info-frame", Handlers::stackinfoframe);
}

void
Commands::updateVariables()
{
    auto &state = Core::state();

    auto currentframe = state->currentStackFrame();
//    auto currentthread = state->currentThread();
    int currentthread = 1;

    state->variables().clear();

    {
    auto cmd = fmt::format("stack-list-variables --frame {0} --thread {1} --simple-values", currentframe, currentthread);
    m_controller->executeCommand(cmd, Handlers::stacklistvariablessimple);
    }

    {
    auto cmd = fmt::format("stack-list-variables --frame {0} --thread {1} --all-values", currentframe, currentthread);
    m_controller->executeCommand(cmd, Handlers::stacklistvariablesall);
    }
}

void
Commands::infoAddress(const std::string &function)
{
    executeConsoleCommand("info address " + function);
}

void
Commands::insertBreakpoint(const std::string &location)
{
    m_controller->executeCommand("break-insert " + location, Handlers::breakinsert);
}

void
Commands::deleteBreakpoint(int number)
{
    m_controller->executeCommand("break-delete " + std::to_string(number), Handlers::breakdelete, number);
}

void
Commands::enableBreakpoint(int number)
{
    m_controller->executeCommand("break-enable " + std::to_string(number), Handlers::breakenable, number);
}

void
Commands::disableBreakpoint(int number)
{
    m_controller->executeCommand("break-disable " + std::to_string(number), Handlers::breakdisable, number);
}

void
Commands::run()
{
    Core::Signals::executeGdbCommand.emit("run");
}

void
Commands::cont()
{
    Core::Signals::executeGdbCommand.emit("continue");
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
    m_controller->executeCommand("exec-next", Handlers::execnext);
}

void
Commands::stepinto()
{
    m_controller->executeCommand("exec-step");
}

void
Commands::stepout()
{
    m_controller->executeCommand("exec-finish");
}

} // namespace Gdb
