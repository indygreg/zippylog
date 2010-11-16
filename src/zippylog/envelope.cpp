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

#include <zippylog/envelope.hpp>
#include <zippylog/platform.hpp>
#include <zippylog/message_registrar.hpp>

#include <google/protobuf/text_format.h>

#include <stdio.h>
#include <sstream>
#include <vector>

namespace zippylog {

using ::std::string;
using ::std::stringstream;
using ::std::vector;

Envelope::Envelope() : messages(NULL), message_count(0)
{
    platform::Time t;
    platform::TimeNow(t);

    this->envelope.set_create_time(t.epoch_micro);
}

Envelope::Envelope(message_t *msg) : messages(NULL), message_count(0)
{
    if (!this->envelope.ParseFromArray(msg->data(), msg->size())) {
        throw "could not parse message";
    }

    int count = this->MessageCount();

    this->messages = new Message *[count];
    this->message_count = count;
    for (int i = 0; i < count; i++) {
        this->messages[i] = NULL;
    }
}

Envelope::Envelope(const string &s) :
    messages(NULL),
    message_count(0)
{
    platform::Time t;
    platform::TimeNow(t);

    this->envelope.set_create_time(t.epoch_micro);
    this->envelope.set_string_value(s);
}

Envelope::~Envelope()
{
    if (this->messages) {
        for (int i = 0; i < this->message_count; i++) {
            if (this->messages[i]) {
                delete this->messages[i];
            }
            this->messages[i] = NULL;
        }
    }

    delete [] this->messages;
    this->messages = NULL;
}

Envelope::Envelope(const Envelope &e)
{
    if (e.message_count > 0) {
        this->message_count = e.message_count;
        this->messages = new Message *[this->message_count];
        for (int i = 0; i < this->message_count; i++) {
            this->messages[i] = NULL;
        }
    }
    else {
        this->message_count = 0;
        this->messages = NULL;
    }

    this->envelope = e.envelope;
}

Envelope & Envelope::operator=(const Envelope &orig)
{
    if (this == &orig) return *this;

    if (orig.message_count > 0) {
        this->message_count = orig.message_count;
        this->messages = new Message *[this->message_count];
        for (int i = 0; i < this->message_count; i++) {
            this->messages[i] = NULL;
        }
    }
    else {
        this->message_count = 0;
        this->messages = NULL;
    }

    this->envelope = orig.envelope;

    return *this;
}

bool Envelope::AddMessage(Message &m, uint32 ns, uint32 enumeration)
{
    if (!this->messages) {
        assert(this->MessageCount() == 0);

        this->messages = new Message *[1];
        this->message_count = 1;
        // don't bother caching it, cuz that would take memory
        this->messages[0] = NULL;
    }
    else {
        // we need to resize the array
        Message **old = this->messages;

        this->messages = new Message *[this->message_count + 1];
        this->messages[this->message_count+1] = NULL;

        for (int i = 0; i < this->message_count; i++) {
            this->messages[i] = old[i];
        }

        delete [] old;
    }

    string buffer;
    if (!m.SerializeToString(&buffer)) return false;

    this->envelope.add_message_namespace(ns);
    this->envelope.add_message_type(enumeration);
    this->envelope.add_message(buffer);

    return true;
}

bool Envelope::ToZmqMessage(message_t &msg)
{
    string buffer;
    this->envelope.SerializeToString(&buffer);
    msg.rebuild(buffer.length());
    memcpy(msg.data(), (void *)buffer.c_str(), buffer.length());

    return true;
}

int Envelope::MessageCount()
{
    return this->envelope.message_size();
}

Message * Envelope::GetMessage(int index)
{
    if (this->MessageCount() < index + 1) return NULL;
    if (this->envelope.message_namespace_size() < index + 1) return NULL;
    if (this->envelope.message_type_size() < index + 1) return NULL;

    assert(this->messages);

    uint32 ns = this->envelope.message_namespace(index);
    uint32 enumeration = this->envelope.message_type(index);

    Message *msg = MessageRegistrar::instance()->get_message(ns, enumeration);
    if (!msg) return NULL;

    string buffer = envelope.message(index);
    msg->ParseFromString(buffer);

    if (!this->messages[index]) this->messages[index] = msg;

    return this->messages[index];
}

bool Envelope::CopyMessage(int index, Envelope &dest)
{
    if (this->envelope.message_size() < index + 1) return false;

    dest.envelope.add_message(this->envelope.message(index));
    dest.envelope.add_message_namespace(this->envelope.message_namespace(index));
    dest.envelope.add_message_type(this->envelope.message_type(index));

    return true;
}

string Envelope::ToString()
{
    ::google::protobuf::TextFormat::Printer printer = ::google::protobuf::TextFormat::Printer();
    printer.SetInitialIndentLevel(4);

    stringstream ss(stringstream::out);

    ss << "Envelope" << ::std::endl;

    if (this->envelope.has_create_time()) {
        platform::Time t;
        platform::UnixMicroTimeToZippyTime(this->envelope.create_time(), t);

        string date = string(30, 0);
        sprintf((char *)date.data(), "%04d-%02d-%02d %02d:%02d:%02d.%06d UTC", t.year, t.mon, t.mday, t.hour, t.min, t.sec, t.usec);
        ss << "  create_time: " << date << ::std::endl;
    }

    if (this->envelope.has_string_value()) {
        ss << "  string: " << this->envelope.string_value() << ::std::endl;
    }

    for (int i = 0; i < this->MessageCount(); i++) {
        string s;
        ::google::protobuf::Message *m = this->GetMessage(i);
        printer.PrintToString(*m, &s);

        ss << "  " << m->GetTypeName() << ::std::endl << s;
    }

    return ss.str();
}

} // namespace