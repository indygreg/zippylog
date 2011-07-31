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

#include <zippylog/envelope.hpp>
#include <zippylog/platform.hpp>
#include <zippylog/message_registrar.hpp>

#include <google/protobuf/text_format.h>

#include <stdio.h>
#include <sstream>
#include <vector>

namespace zippylog {

using ::google::protobuf::Message;
using ::std::invalid_argument;
using ::std::string;
using ::std::stringstream;
using ::std::vector;
using ::zmq::message_t;

Envelope::Envelope() : messages(NULL), messages_size(0)
{
    platform::Time t;
    platform::TimeNow(t);

    this->envelope.set_create_time(t.epoch_micro);
}

Envelope::Envelope(message_t &msg, uint32 offset)
{
    if (!msg.size()) throw invalid_argument("0MQ message is empty");
    if (offset + 1 >= msg.size()) throw invalid_argument("specified offset is larger than message");

    this->InitializeFromBuffer((const void *)((char *)msg.data() + offset), msg.size() - offset);
}

Envelope::Envelope(const void * data, int size) : messages(NULL), messages_size(0)
{
    if (!data) throw invalid_argument("NULL data pointer");
    if (!size) throw invalid_argument("0 size data buffer");
    if (size < 0) throw invalid_argument("size <0: " + size);

    this->InitializeFromBuffer(data, size);
}

Envelope::Envelope(string const &s) :
    messages(NULL),
    messages_size(0)
{
    platform::Time t;
    platform::TimeNow(t);

    this->envelope.set_create_time(t.epoch_micro);
    this->envelope.set_string_value(s);
}

Envelope::~Envelope()
{
    if (this->messages) {
        for (int i = 0; i < this->messages_size; i++) {
            if (this->messages[i]) {
                delete this->messages[i];
            }
            this->messages[i] = NULL;
        }

        delete [] this->messages;
    }
}

Envelope::Envelope(Envelope const &e)
    : messages(NULL), messages_size(0)
{
    if (e.messages_size > 0) {
        this->messages_size = e.messages_size;
        this->messages = new Message *[this->messages_size];
        for (int i = 0; i < this->messages_size; i++) {
            this->messages[i] = NULL;
        }
    }

    this->envelope.CopyFrom(e.envelope);
}

Envelope & Envelope::operator=(Envelope const &orig)
{
    if (this == &orig) return *this;

    if (orig.messages_size > 0) {
        this->messages_size = orig.messages_size;
        this->messages = new Message *[this->messages_size];
        for (int i = 0; i < this->messages_size; i++) {
            this->messages[i] = NULL;
        }
    }
    else {
        this->messages_size = 0;
        this->messages = NULL;
    }

    this->envelope = orig.envelope;

    return *this;
}

bool Envelope::operator==(Envelope const &other) const
{
    if (this == &other) return true;

    string s1, s2;
    this->Serialize(s1);
    other.Serialize(s2);

    return s1.compare(s2) == 0;
}

bool Envelope::operator!=(Envelope const &other) const
{
    return !(*this == other);
}

void Envelope::InitializeFromBuffer(const void * data, int size)
{
    if (!this->envelope.ParseFromArray(data, size)) {
        throw DeserializeException();
    }

    int count = this->MessageCount();

    this->messages = new Message *[count];
    this->messages_size = count;
    for (int i = 0; i < count; i++) {
        this->messages[i] = NULL;
    }
}

bool Envelope::Serialize(string &s) const
{
    return this->envelope.AppendToString(&s);
}

bool Envelope::AddMessage(Message &m, uint32 ns, uint32 enumeration)
{
    if (!this->messages) {
        assert(this->MessageCount() == 0);

        this->messages = new Message *[1];
        this->messages_size = 1;
        // don't bother caching it, cuz that would take memory
        this->messages[0] = NULL;
    }
    else {
        // we need to resize the array
        Message **old = this->messages;

        this->messages = new Message *[this->messages_size + 1];
        this->messages[this->messages_size] = NULL;

        for (int i = 0; i < this->messages_size; i++) {
            this->messages[i] = old[i];
        }

        this->messages_size++;

        delete [] old;
    }

    string buffer;
    if (!m.SerializeToString(&buffer)) return false;

    this->envelope.add_message_namespace(ns);
    this->envelope.add_message_type(enumeration);
    this->envelope.add_message(buffer);

    return true;
}

bool Envelope::ParseFromCodedInputStream(::google::protobuf::io::CodedInputStream &cis)
{
    if (this->messages) {
        for (int i = 0; i < this->messages_size; i++) {
            if (this->messages[i]) {
                delete this->messages[i];
            }
        }

        delete [] this->messages;
        this->messages = NULL;
        this->messages_size = 0;
    }

    bool result = this->envelope.ParseFromCodedStream(&cis) && cis.ConsumedEntireMessage();

    if (result) {
        int count = this->MessageCount();

        this->messages = new Message *[count];
        for (int i = 0; i < count; i++) {
            this->messages[i] = NULL;
        }
        this->messages_size = count;
    }

    return result;
}

bool Envelope::ToZmqMessage(message_t &msg)
{
    string buffer;
    if (!this->envelope.AppendToString(&buffer)) return false;
    msg.rebuild(buffer.length());
    memcpy(msg.data(), buffer.data(), buffer.length());

    return true;
}

bool Envelope::ToProtocolZmqMessage(message_t &msg) const
{
    string buffer(1, 0x01);
    if (!this->envelope.AppendToString(&buffer)) return false;
    msg.rebuild(buffer.length());
    memcpy(msg.data(), buffer.data(), buffer.length());

    return true;
}

int Envelope::MessageCount()
{
    return this->envelope.message_size();
}

Message * Envelope::GetMessage(int index)
{
    if (index < 0) throw invalid_argument("index must be non-negative");

    if (this->MessageCount() < index + 1) return NULL;
    if (this->envelope.message_namespace_size() < index + 1) return NULL;
    if (this->envelope.message_type_size() < index + 1) return NULL;

    // the messages cache should be initialized at construction time and
    // modified whenever messages are added
    assert(this->messages);

    if (this->messages[index])
        return this->messages[index];

    uint32 ns = this->envelope.message_namespace(index);
    uint32 enumeration = this->envelope.message_type(index);

    Message *msg = MessageRegistrar::instance()->GetMessage(ns, enumeration);
    if (!msg) return NULL;

    string buffer = envelope.message(index);
    if (!msg->ParseFromString(buffer)) {
        delete msg;
        return NULL;
    }

    this->messages[index] = msg;

    return this->messages[index];
}

bool Envelope::CopyMessage(int index, Envelope &dest) const
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
        if (m) {
            printer.PrintToString(*m, &s);
            ss << "  " << m->GetTypeName() << ::std::endl << s;
        }
    }

    return ss.str();
}

} // namespace