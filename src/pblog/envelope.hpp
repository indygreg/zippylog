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

#ifndef PBLOG_ENVELOPE_HPP_
#define PBLOG_ENVELOPE_HPP_

#include <pblog/pblog.h>
#include <pblog/message.pb.h>

#include <google/protobuf/message.h>
#include <zmq.hpp>

namespace pblog {

using ::google::protobuf::Message;
using ::google::protobuf::uint32;
using ::zmq::message_t;

class PBLOG_EXPORT Envelope {
    public:
        // construct an empty envelope
        Envelope();

        // construct from a 0MQ message
        Envelope(message_t *msg);

        // adds a protobuf messsage to the payload.
        //
        // message is effectively copied to the envelope. any modifications
        // after adding will not be reflected. passed message can be
        // deleted as soon as function returns.
        //
        // We require namespace and enumeration now. If we can get around
        // inefficient castings, we'll likely create a new overload
        bool add_message(Message *m, uint32 ns, uint32 enumeration);

        message::Envelope envelope;

        /* adds the envelope to a ZMQ message and returns the message */
        message_t * to_zmq_message();

        Message * get_message(int index);
};

} // namespace

#endif