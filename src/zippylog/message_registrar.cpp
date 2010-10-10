//  Copyright 2010 Gregory Szorc
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

#include <zippylog/message_registrar.hpp>
#include <zippylog/messages.hpp>

namespace zippylog {

MessageRegistrar::MessageRegistrar()
{
    register_known_messages();
}

MessageRegistrar::~MessageRegistrar()
{
    this->Cleanup();
}

MessageRegistrar * MessageRegistrar::instance()
{
    static MessageRegistrar registrar;
    return &registrar;
}

bool MessageRegistrar::register_message(uint32 ns, uint32 enumeration, Message *instance)
{
    pair<uint32, uint32> p(ns, enumeration);
    map<pair<uint32, uint32>, Message *>::iterator iter = this->_types.find(p);

    if (iter != this->_types.end()) {
        return false;
    }

    this->_types.insert(make_pair(p, instance));

    return true;
}

Message * MessageRegistrar::get_message(uint32 ns, uint32 enumeration)
{
    pair<uint32, uint32> p(ns, enumeration);
    map<pair<uint32, uint32>, Message *>::iterator iter = this->_types.find(p);

    if (iter != this->_types.end()) {
        return iter->second->New();
    }
    return NULL;
}

void MessageRegistrar::Cleanup()
{
    map<pair<uint32, uint32>, Message *>::iterator iter = this->_types.begin();
    for (; iter != this->_types.end(); iter++) {
        delete iter->second;
    }
    this->_types.clear();
}

} // namespace