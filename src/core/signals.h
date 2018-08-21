/*
 * signals.h
 *
 *  Created on: Jul 19, 2017
 *      Author: jasonr
 */

#ifndef CORE_SIGNAL_H_
#define CORE_SIGNAL_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string>
#include <functional>
#include <map>

#include "state.h"

namespace Core
{

// from http://simmesimme.github.io/tutorials/2015/09/20/signal-slot

// A signal object may call multiple slots with the
// same signature. You can connect functions to the signal
// which will be called when the emit() method on the
// signal object is invoked. Any argument passed to emit()
// will be passed to the given functions.

template<typename ... Args>
class Signal
{

public:

    Signal()
        : current_id_(0)
    {
    }

    // copy creates new signal
    Signal(Signal const& other)
        : current_id_(0)
    {
    }

    // connects a member function to this Signal
    template<typename T>
    int connect(T *inst, void (T::*func)(Args...))
    {
        return connect([=](Args... args)
        {
            (inst->*func)(args...);
        });
    }

    // connects a const member function to this Signal
    template<typename T>
    int connect(T *inst, void (T::*func)(Args...) const)
    {
        return connect([=](Args... args)
        {
            (inst->*func)(args...);
        });
    }

    // connects a std::function to the signal. The returned
    // value can be used to disconnect the function again
    int connect(std::function<void(Args...)> const& slot) const
    {
        slots_.insert(std::make_pair(++current_id_, slot));
        return current_id_;
    }

    // disconnects a previously connected function
    void disconnect(int id) const
    {
        slots_.erase(id);
    }

    // disconnects all previously connected functions
    void disconnect_all() const
    {
        slots_.clear();
    }

    void operator()(Args... p)
    {
        for (auto it : slots_)
        {
            it.second(p...);
        }
    }

    // assignment creates new Signal
    Signal& operator=(Signal const& other)
    {
        disconnect_all();
    }

private:
    mutable std::map<int, std::function<void(Args...)>> slots_;
    mutable int current_id_;
};

namespace Signals
{

// request that the source editor load the contents of filename
extern Signal<const std::string & /*filename*/> loadEditorSource;

// emitted when quit is requested
extern Signal<> quitRequested;

// set cursor position of the source editor
extern Signal<int /*x*/, int /*y*/> setCursorPosition;

// update the source editor's gutter marker on row
extern Signal<int /*row*/> updateGutterMarker;

// set the debugger's current location
extern Signal<const Location & /*location*/> setCurrentLocation;

// remove the current location marker from the source editor
extern Signal<> clearCurrentLocation;

// append text to various output widgets
extern Signal<const std::string & /*text*/> appendConsoleText;
extern Signal<const std::string & /*text*/> appendLogText;
extern Signal<const std::string & /*text*/> appendOutputText;

// set statusbar text
extern Signal<const std::string & /*text*/> setStatusbarText;

// emitted when gdb's running state is updated
extern Signal<> debuggerStateUpdated;

// emitted when the global callstack is updated
extern Signal<> callStackUpdated;

// emitted when the global breakpoint list is updated
extern Signal<> breakPointsUpdated;

// emitted when the list of source files is updated
extern Signal<> sourceListUpdated;

// emitted when the list of function names is updated
extern Signal<> functionListUpdated;

}

}

#endif // CORE_SIGNAL_H_
