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

#ifndef ZIPPYLOG_ENVELOPE_HPP_
#define ZIPPYLOG_ENVELOPE_HPP_

#include <zippylog/zippylog.h>
#include <zippylog/message.pb.h>

#include <google/protobuf/message.h>
#include <zmq.hpp>

namespace zippylog {

using ::google::protobuf::Message;
using ::zmq::message_t;

class ZIPPYLOG_EXPORT Envelope {
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
        bool AddMessage(Message &m, uint32 ns, uint32 enumeration);

        message::Envelope envelope;

        bool merge_from_zmq_message(message_t *msg);

        // serializes the envelope into a 0MQ message
        // existing message content will be overwritten
        bool ToZmqMessage(message_t &msg);

        int number_messages();
        inline uint32 message_namespace(int index)
        {
            return this->envelope.message_namespace(index);
        }
        inline uint32 message_type(int index)
        {
            return this->envelope.message_type(index);
        }

        Message * get_message(int index);

        // copy a message to another envelope
        bool CopyMessage(int index, Envelope &dest);
};

} // namespace

#endif