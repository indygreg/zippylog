//  Copyright 2011 Gregory Szorc
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#ifndef ZIPPYLOG_DEVICE_HPP_
#define ZIPPYLOG_DEVICE_HPP_

#include <zippylog/zippylog.hpp>
#include <zippylog/platform.hpp>

namespace zippylog {
namespace device {

/// Represents the result of a Pump() operation in a Device
///
/// See Device::Pump() for more
class ZIPPYLOG_EXPORT PumpResult {
public:
    /// Construct an instance which reflects that work was done
    static PumpResult MakeWorkDone();

    /// Construct an instance which reflects that no work was done
    static PumpResult MakeNoWorkDone();

    /// Construct an instance which relfects an error was encountered
    static PumpResult MakeError();

    /// Whether an error was encountered
    inline bool IsError() { return this->is_error; }

private:
    PumpResult() : is_error(false) { }

    bool is_error;
};

/// An entity that performs work
///
/// Devices are entities that perform specific functionality. Devices
/// communicate with other entities and devices via 0MQ sockets. In other
/// words, devices send their output via 0MQ sockets and typically receive
/// their input via 0MQ sockets. This being said, the behavior is not
/// enforced, so devices could operate using other means, for example.
///
/// This class is an abstract base class to be used by real devices. It
/// defines a core API and a set of functions which must be defined on
/// all derived devices.
///
/// In 0MQ 2.0.x, 0MQ sockets cannot be migrated between threads. Since
/// devices are constructed on one thread and can run on another, it is
/// important for the constructor not to initialize 0MQ sockets. Instead,
/// it is recommended to initialize sockets inside the OnFirstRun() handler,
/// which will only be called once.
///
class ZIPPYLOG_EXPORT Device {
public:
    /// Base constructor
    ///
    /// This should be called by child classes in their constructors.
    ///
    /// The constructor accepts a pointer to a conditional wait instance which
    /// is used to determine if the device should run. When this semaphore
    /// enters the signaled state, the device stops running or will refuse to
    /// run.
    ///
    /// @param cw Semaphore that determines whether device should run
    Device(::zippylog::platform::ConditionalWait *cw);

    /// Base class destructor
    virtual ~Device();

    /// Performs pending work
    ///
    /// This is the most important function in a device because it is what
    /// triggers the device to perform work. From within this function, an
    /// implementation should process pending work or wait for work to become
    /// available until the timeout specified.
    ///
    /// It is important for devices to honor the timeout threshold, otherwise
    /// the device breaks its API contract, which may have negative impact on
    /// performance, latency, etc.
    ///
    /// @param timeout_milliseconds How long to wait for work to become
    /// available before giving up
    virtual PumpResult Pump(int32 timeout_milliseconds) = 0;

    /// Runs the device forever
    ///
    /// The device runs until the semaphore defined by the constructor
    /// signals. If the semaphore has not been defined, an error is thrown.
    ///
    /// Calling this function is equivalent to calling Pump() inside an
    /// infinite loop.
    void Run();

    /// Runs the device asynchronously on a new thread
    ///
    /// This function will spawn a new thread and have the device execute
    /// continuously on that thread.
    ///
    /// The function does not return until after OnRunStart() finishes.
    /// This gives devices an opportunity to fully initialize on their new
    /// thread before program execution continues.
    ///
    /// To stop this thread, call StopAsync().
    void RunAsync();

    /// Stops the device from executing on a background thread
    ///
    /// This is the inverse of RunAsync(). It will signal the device to stop
    /// and terminate the spawned thread. When the function returns, the
    /// thread is guaranteed to be shut down.
    ///
    /// @param join_timeout How long to wait for the thread to join before
    /// resorting to an abort, in microseconds
    void StopAsync(int32 join_timeout = 1000000);

    /// Query to see if the device is running
    ///
    /// @return true if device is running. false if not
    inline bool IsRunning() { return this->running; }

protected:

    /// Called when the device first runs
    ///
    /// This can be used to perform one-time object setup. Execution occurs
    /// on the new thread if running asynchronously.
    virtual void OnFirstRun();

    /// Called at the start of Run() and RunAsync()
    ///
    /// Child classes should define this to perform commands every time Run()
    /// is called.
    virtual void OnRunStart();

    /// Called at the end of Run() and StopAsync()
    ///
    /// This function should perform cleanup actions
    virtual void OnRunFinish();

private:
    /// Function that gets executed when RunAsync() is called
    ///
    /// @param data this pointer to Device instance
    static void * AsyncExecStart(void *data);

    /// Background thread device executes on
    ///
    /// This will only be defined if RunAsync() is called.
    ::zippylog::platform::Thread *thread;

    /// Semaphore used to determine if device is active
    ::zippylog::platform::ConditionalWait *cw;

    /// Whether the device is running
    bool running;

    /// Whether Run() has executed before
    bool has_ran;

    /// Used by RunAsync() so function blocks until after OnRunStart() is
    /// finished
    ::zippylog::platform::ConditionalWait async_wait;

    /// Disable copy constructor and assignment operator
    Device(Device const &orig);
    Device & operator=(Device const &orig);
};

}} // namespace

#endif // file include
