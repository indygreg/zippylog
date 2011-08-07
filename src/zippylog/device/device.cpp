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

Device::Device(ConditionalWait *cw) : thread(NULL), cw(cw)
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

void Device::Run()
{
    // if semaphore is signaled, don't do anything
    if (this->cw->Wait(0)) return;

    this->running = true;

    this->OnRunStart();

    while (!this->cw->Wait(0)) {
        /// @todo timeout should come from configuration
        this->Pump(10000);
    }

    this->OnRunFinish();

    this->running = false;
}

void Device::RunAsync()
{
    if (this->cw->Wait(0)) return;

    // run the setup synchronously
    this->OnRunStart();

    if (this->thread) {
        throw Exception("device is already running asynchronously");
    }

    this->thread = new Thread(AsyncExecStart, (void *)this);
}

void Device::StopAsync(int32 timeout)
{
    if (!this->thread) {
        throw Exception("device is not running asynchronously");
    }

    this->cw->Signal();

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
    /// @todo do error handling properly
    try {
        Device *device = (Device *)data;
        device->Run();
        device->OnRunFinish();
    }
    catch (::std::exception e) {
        string error = e.what();
        return (void *)1;
    }

    return (void *)0;
}

}} // namespace