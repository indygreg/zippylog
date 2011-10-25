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

#include <zippylog/device/device.hpp>

#include <zmq.hpp>
#include <string>

using ::std::invalid_argument;
using ::std::string;
using ::zippylog::platform::ConditionalWait;
using ::zippylog::platform::Thread;

namespace zippylog {
namespace device {

PumpResult PumpResult::MakeError()
{
    PumpResult result;
    result.is_error = true;

    return result;
}

PumpResult PumpResult::MakeWorkDone()
{
    PumpResult result;

    return result;
}

PumpResult PumpResult::MakeNoWorkDone()
{
    PumpResult result;

    return result;
}

Device::Device(::zippylog::platform::ConditionalWait *cw) :
    thread(NULL),
    cw(cw),
    running(false),
    has_ran(false)
{
    if (!cw) {
        throw invalid_argument("ConditionalWait parameter not defined");
    }
}

Device::~Device()
{
    if (this->thread) {
        if (!this->thread->Join()) {
            this->thread->Abort();
        }

        delete this->thread;
    }
}

void Device::OnRunStart() { }
void Device::OnRunFinish() { }
void Device::OnFirstRun() { }

void Device::Run()
{
    // if semaphore is signaled, don't do anything
    if (this->cw->Wait(0)) return;

    if (!this->has_ran) this->OnFirstRun();

    this->OnRunStart();

    this->running = true;
    this->has_ran = true;
    this->async_wait.Signal();

    while (!this->cw->Wait(0)) {
        /// @todo timeout should come from configuration
        this->Pump(10);
    }

    this->OnRunFinish();

    this->running = false;
}

void Device::RunAsync()
{
    if (this->cw->Wait(0)) return;

    if (this->thread) {
        throw Exception("device is already running asynchronously");
    }

    // just in case
    this->async_wait.Reset();

    this->thread = new Thread(AsyncExecStart, (void *)this);

    // wait for OnRunStart() to finish
    this->async_wait.Wait(-1);
}

void Device::StopAsync(int32)
{
    if (!this->thread) {
        throw Exception("device is not running asynchronously");
    }

    this->cw->Signal();

    /// @todo use timeout parameter

    if (this->thread->Alive()) {
        if (!this->thread->Join()) {
            this->thread->Abort();
        }

        delete this->thread;
        this->thread = NULL;
    }

    this->running = false;
}

void * Device::AsyncExecStart(void *data)
{
    Device *device = (Device *)data;
    void * ret = (void *)0;

    /// @todo do error handling properly
    try {
        device->Run();
    }
    catch (::zmq::error_t e) {
        string error = e.what();
        ret = (void *)1;
    }
    catch (::std::exception e) {
        device->running = false;
        string error = e.what();
        ret = (void *)1;
    }
    catch ( ... ) { }

    try {
        device->OnRunFinish();
    }
    catch ( ... ) {}

    device->running = false;

    return ret;
}

}} // namespace
