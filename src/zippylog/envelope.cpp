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

#include <pblog/envelope.hpp>
#include <pblog/message_registrar.hpp>

#include <string>

namespace pblog {

using ::std::string;

Envelope::Envelope()
{
    ;
}

Envelope::Envelope(message_t *msg)
{
    this->envelope.ParseFromArray(msg->data(), msg->size());
}

bool Envelope::add_message(Message *m, uint32 ns, uint32 enumeration)
{
    string buffer;
    if (!m->SerializeToString(&buffer)) return false;

    this->envelope.add_message_namespace(ns);
    this->envelope.add_message_type(enumeration);
    this->envelope.add_message(buffer);

    return true;
}

message_t * Envelope::to_zmq_message()
{
    string buffer;
    this->envelope.SerializeToString(&buffer);

    message_t *msg = new message_t(buffer.length());
    memcpy(msg->data(), (void *)buffer.c_str(), buffer.length());

    return msg;
}

bool Envelope::merge_from_zmq_message(message_t *msg)
{
    return this->envelope.ParseFromArray(msg->data(), msg->size());
}

int Envelope::number_messages()
{
    return this->envelope.message_size();
}

Message * Envelope::get_message(int index)
{
    Message *msg = NULL;

    if (this->envelope.message_size() < index + 1) {
        return NULL;
    }

    if (this->envelope.message_namespace_size() < index + 1) {
        return NULL;
    }

    if (this->envelope.message_type_size() < index + 1) {
        return NULL;
    }

    uint32 ns = this->envelope.message_namespace(index);
    uint32 enumeration = this->envelope.message_type(index);

    msg = MessageRegistrar::instance()->get_message(ns, enumeration);
    if (!msg) return msg;

    string buffer = envelope.message(index);
    msg->ParseFromString(buffer);

    return msg;
}

} // namespace