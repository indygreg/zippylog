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

#include <zippylog/zippylog.hpp>
#include <zippylog/message_registrar.hpp>
#include <zippylog/messages.hpp>

#include <google/protobuf/stubs/common.h>

namespace zippylog {

void initialize_library()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // populate the registrar instance
    ::zippylog::MessageRegistrar::instance();
    ::zippylog::register_known_messages();
}

void shutdown_library()
{
    ::google::protobuf::ShutdownProtobufLibrary();

    ::zippylog::MessageRegistrar::TerminateInstance();
}

} // namespace
