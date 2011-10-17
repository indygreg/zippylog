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
#include <zippylog/core.pb.h>
#include <zippylog/platform.hpp>
#include <zippylog/message_registrar.hpp>

#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/text_format.h>

#include <stdio.h>
#include <sstream>
#include <vector>

namespace zippylog {

using ::google::protobuf::FieldDescriptor;
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

Envelope::Envelope(::zmq::message_t &msg, uint32 offset) : messages(NULL), messages_size(0)
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

Envelope::Envelope(::std::string const &s) : messages(NULL), messages_size(0)
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

Envelope::Envelope(Envelope const &e) : messages(NULL), messages_size(0)
{
    this->envelope.CopyFrom(e.envelope);
}

Envelope & Envelope::operator=(Envelope const &orig)
{
    if (this == &orig) return *this;

    this->messages_size = 0;
    this->messages = NULL;

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
}

bool Envelope::Serialize(::std::string &s) const
{
    return this->envelope.AppendToString(&s);
}

bool Envelope::AddMessage(Message &m, uint32 ns, uint32 enumeration)
{
    string buffer;
    if (!m.SerializeToString(&buffer)) return false;

    this->envelope.add_message_namespace(ns);
    this->envelope.add_message_type(enumeration);
    this->envelope.add_message(buffer);

    return true;
}

bool Envelope::ParseFromCodedInputStream(::google::protobuf::io::CodedInputStream &cis)
{
    // Wipe out existing cached messages.
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

int Envelope::MessageCount() const
{
    return this->envelope.message_size();
}

Message * Envelope::GetMessage(int index)
{
    if (index < 0) throw invalid_argument("index must be non-negative");

    if (this->envelope.message_size() < index + 1) return NULL;
    if (this->envelope.message_namespace_size() < index + 1) return NULL;
    if (this->envelope.message_type_size() < index + 1) return NULL;

    int count = this->envelope.message_size();
    if (!this->messages) {
        assert(!this->messages_size);

        this->messages = new Message *[count];
        this->messages_size = count;

        for (int i = 0; i < count; i++) {
            this->messages[i] = NULL;
        }
    }
    // Resize messages array as necessary. We only check the greater than case
    // because RemoveMessage will properly set excess values to NULL upon
    // removal.
    else if (count > this->messages_size) {
        Message **old = this->messages;

        this->messages = new Message *[count];

        for (int i = 0; i < count; i++) {
            this->messages[i] = i < this->messages_size ? old[i] : NULL;
        }

        this->messages_size = count;
        delete [] old;
    }

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
    if (index < 0) throw invalid_argument("index must be non-negative");

    if (this->envelope.message_size() < index + 1) return false;

    dest.envelope.add_message(this->envelope.message(index));
    dest.envelope.add_message_namespace(this->envelope.message_namespace(index));
    dest.envelope.add_message_type(this->envelope.message_type(index));

    return true;
}

void Envelope::Clear()
{
    uint64 create_time = this->envelope.has_create_time() ?
        this->envelope.create_time() : 0;

    this->envelope.Clear();

    if (this->messages) {
        for (int i = 0; i < this->messages_size; i++) {
            if (this->messages[i]) {
                delete this->messages[i];
                this->messages[i] = NULL;
            }
        }

        delete [] this->messages;
        this->messages = NULL;
    }
    this->messages_size = 0;

    this->envelope.set_create_time(create_time);
}

bool Envelope::RemoveMessage(int32 index)
{
    if (index < 0) throw invalid_argument("index must be non-negative");

    int initial_size = this->envelope.message_size();

    if (initial_size < index + 1) return false;

    // The protocol buffer API doesn't provide a way to easily remove elements
    // from a repeated field, hence the complication.
    for (int i = 0; i < initial_size; i++) {
        if (i <= index) continue;

        // else we move everything down a slot
        this->envelope.set_message(i - 1, this->envelope.message(i));
        this->envelope.set_message_namespace(i - 1, this->envelope.message_namespace(i));
        this->envelope.set_message_type(i - 1, this->envelope.message_type(i));

        if (this->messages && this->messages_size >= i) {
            this->messages[i - 1] = this->messages[i];
        }
    }

    this->envelope.mutable_message()->RemoveLast();
    this->envelope.mutable_message_namespace()->RemoveLast();
    this->envelope.mutable_message_type()->RemoveLast();

    if (this->messages && this->messages_size >= initial_size) {
        this->messages[initial_size - 1] = NULL;
    }

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
            ss << "  ";
            if (!CustomPrintMessage(m, ss)) {
               printer.PrintToString(*m, &s);
               ss << m->GetTypeName() << ::std::endl << s;
            }
        }
    }

    return ss.str();
}

/// TODO generate per-message formatting logic at .proto compilation time so
/// we don't incur run-time penalty
bool Envelope::CustomPrintMessage(const ::google::protobuf::Message *m,
                                  ::std::ostream &s)
{
    ::google::protobuf::Descriptor const *descriptor = m->GetDescriptor();
    ::google::protobuf::MessageOptions options = descriptor->options();

    if (!options.HasExtension(zippylog::formatter)) return false;

    ::google::protobuf::Reflection const *reflection = m->GetReflection();

    string formatter = options.GetExtension(zippylog::formatter);

    string::size_type length = formatter.length();
    string::size_type i = 0;
    do {
        string::value_type c = formatter[i];

        if (c != '%') {
            s << c;
            i++;
            continue;
        }

        // This should never happen, but we guard against stupidity.
        if (i >= length - 1) {
            s << c;
            i++;
            continue;
        }

        string::value_type field = formatter[++i];
        int field_n = atoi(&field);

        FieldDescriptor const * fd = descriptor->FindFieldByNumber(field_n);

        /// TODO implement better formatting
        /// e.g. protobuf puts double quotes around strings, which we don't
        /// want.
        if (fd->is_repeated()) {
            int size = reflection->FieldSize(*m, fd);
            for (int fi = 0; fi < size; fi++) {
                string v;
                ::google::protobuf::TextFormat::PrintFieldValueToString(*m, fd, fi, &v);
                s << v;
            }
        }
        else {
            string v;
            ::google::protobuf::TextFormat::PrintFieldValueToString(*m, fd, -1, &v);
            s << v;
        }

        i++;

    } while (i < length);

    s << ::std::endl;

    return true;
}

} // namespace