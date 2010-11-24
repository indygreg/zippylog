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

#include <zippylog/zippylog.hpp>
#include <zippylog/message.pb.h>

#include <google/protobuf/message.h>
#include <zmq.hpp>

#include <string>

// windows.h defines GetMessage() as a macro, which craps on us.
// here, we shovel shit
#if defined(WINDOWS) && defined(GetMessage)

inline BOOL GetMessage_Windows(LPMSG msg, HWND hwnd, UINT min, UINT max) {
    return GetMessage(msg, hwnd, min, max);
}

#undef GetMessage

inline BOOL GetMessage(LPMSG msg, HWND hwnd, UINT min, UINT max) {
    return GetMessage_Windows(msg, hwnd, min, max);
}

#endif

namespace zippylog {

class ZIPPYLOG_EXPORT Envelope {
    public:
        // construct an empty envelope
        Envelope();

        // construct from a 0MQ message
        Envelope(::zmq::message_t *msg);

        /// construct an envelope having string data
        ///
        /// This populates the string_value field of the envelope. It does
        /// not build an envelope from the serialized protocol buffer data
        /// stored in the passed string
        Envelope(const ::std::string &s);

        ~Envelope();

        Envelope & operator=(const Envelope &orig);
        Envelope(const Envelope &e);

        // adds a protobuf messsage to the payload.
        //
        // message is effectively copied to the envelope. any modifications
        // after adding will not be reflected. passed message can be
        // deleted as soon as function returns.
        //
        // We require namespace and enumeration now. If we can get around
        // inefficient castings, we'll likely create a new overload
        bool AddMessage(::google::protobuf::Message &m, uint32 ns, uint32 enumeration);

        message::Envelope envelope;

        // serializes the envelope into a 0MQ message
        // existing message content will be overwritten
        bool ToZmqMessage(::zmq::message_t &msg);

        int MessageCount();

        inline uint32 MessageNamespace(int index)
        {
            return this->envelope.message_namespace(index);
        }

        inline uint32 MessageType(int index)
        {
            return this->envelope.message_type(index);
        }

        // obtain the protocol buffer message at given index
        // the returned pointer is owned by the envelope instance
        // the memory won't be accessible once the envelope is destroyed
        // therefore, the caller should NOT free it
        // if the index does not exist, NULL will be returned
        ::google::protobuf::Message * GetMessage(int index);

        // copy a message to another envelope
        bool CopyMessage(int index, Envelope &dest);

        ::std::string ToString();

    protected:
        // holds pointer to dynamic array
        ::google::protobuf::Message ** messages;
        int message_count;
};

} // namespace

#endif