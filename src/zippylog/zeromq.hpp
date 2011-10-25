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

#ifndef ZIPPYLOG_ZEROMQ_HPP_
#define ZIPPYLOG_ZEROMQ_HPP_

#include <zippylog/zippylog.hpp>
#include <zippylog/envelope.hpp>
#include <zmq.hpp>

#include <string>
#include <vector>

namespace zippylog {
namespace zeromq {

/// Represents a container for 0MQ messages
///
/// This container is used as part of the Tx and Rx APIs. 0MQ messages will
/// automatically be garbage collected as part of the container's lifetime.
/// This is much easier than passing around a vector<message_t *> which must
/// be managed by the caller.
class ZIPPYLOG_EXPORT MessageContainer {
public:
    /// Construct a new, empty container
    MessageContainer();
    ~MessageContainer();

    /// Clear the contents of the container
    void Clear();

    /// Adds an identity message to the container
    ///
    /// Ownership of the memory address of the message is transferred to the container
    void AddIdentity(::zmq::message_t *msg);

    /// Adds a message to the container
    ///
    /// Ownership of the memory address of the message is transferred to the container
    void AddMessage(::zmq::message_t *msg);

    /// The number of identities for the message sequence
    inline ::std::vector< ::std::string >::size_type IdentitiesSize() const { return this->identities.size(); }

    /// Obtain a specified identity
    ///
    /// @param i Offset to retrieve. Initial is 0
    /// @return Identity string
    inline ::std::string GetIdentity(int i) const { return this->identities[i]; }

    /// Obtain all the identities
    ///
    /// @return Set of all the identities
    inline ::std::vector< ::std::string > const & GetIdentities() const { return this->identities; }

    /// Obtain the number of messages in the container
    ///
    /// @return The number of messages in the container
    inline ::std::vector< ::zmq::message_t *>::size_type MessagesSize() const { return this->messages.size(); }

    /// Obtains the 0MQ message at the specified index
    ///
    /// The index of the first message is 0.
    ///
    /// The memory backing the message is owned by the container and has a
    /// lifetime the same as the container's.
    inline ::zmq::message_t * GetMessage(int i) const { return this->messages[i]; }

    /// Obtains all of the payload 0MQ messages in the container
    ///
    /// @return vector of messages
    inline ::std::vector< ::zmq::message_t *> const & GetMessages() const { return this->messages; }

    /// Override subscript operator to return message at offset
    inline ::zmq::message_t * const operator[](const int index) const { return this->messages[index]; }

protected:
    /// Holds the socket identities
    ::std::vector< ::std::string > identities;

    /// Holds the raw payload messages
    ::std::vector< ::zmq::message_t *> messages;


private:
    // forbid copy constructor and assignment operator
    // we could support these if we wanted to. we are currently lazy
    MessageContainer(MessageContainer const &orig);
    MessageContainer & operator=(MessageContainer const &orig);
};

/// Sends an envelope over a socket
///
/// @param socket Socket to send over
/// @param e Envelope to send
/// @param is_protocol Serialize as a protocol message
/// @param flags 0MQ send flags
/// @return 0 on send failure (try again), -1 on serialization failure, 1 success
int SendEnvelope(::zmq::socket_t &socket, Envelope &e, bool is_protocol, int flags);

/// Sends an envelope over a socket with message identities
///
/// This is a convenience method to sends an envelope over an XREQ/XREP socket
/// with message identities.
///
/// The identities will be sent as message lebels. The flags for the envelope
/// message are always from the parameter flags. If the ZMQ_DONTWAIT flag is set, it
/// is set on all message parts.
int SendEnvelope(::zmq::socket_t &socket,
                 ::std::vector< ::std::string >  const &identities,
                 Envelope &e,
                 bool is_protocol,
                 int flags);

/// Sends an envelope over a socket with identities
///
/// This is an overloaded version of the above to support socket pointers, not
/// references.
inline int SendEnvelope(::zmq::socket_t * const socket,
                 ::std::vector< ::std::string > const &identities,
                 Envelope &e,
                 bool is_protocol,
                 int flags)
{
    return SendEnvelope(*socket, identities, e, is_protocol, flags);
}

/// Sends an envelope over a socket
///
/// @param socket Socket to send envelope on
/// @param e Envelope to send over socket
/// @param is_protocol Whether to serialize the envelope as a zippylog protocol message
/// @param flags 0MQ send flags
inline int SendEnvelope(::zmq::socket_t * const socket,
                        Envelope &e,
                        bool is_protocol,
                        int flags)
{
    ::std::vector< ::std::string > i;
    return SendEnvelope(*socket, i, e, is_protocol, flags);
}

/// Sends multiple envelopes over a socket with identities
///
/// This will send a single mutli-part 0MQ message. The specified identities
/// will be sent as message labels.
///
/// @param socket Socket to send data over
/// @param identities Identities for message routing
/// @param envelopes Envelopes to send over the socket
/// @param is_protocol Whether to serialize the envelopes as zippylog protocol messages
/// @param flags 0MQ send flags
int SendEnvelopes(::zmq::socket_t &socket,
                  ::std::vector< ::std::string > const &identities,
                  ::std::vector<Envelope> &envelopes,
                  bool is_protocol,
                  int flags);

/// Receives a message on a 0MQ socket
///
/// On success, the message container will be populated with the received
/// message. The message can be a multipart 0MQ message.
bool ReceiveMessage(::zmq::socket_t &socket, MessageContainer &container, int flags=0);

/// Receives a multi-part message on one socket and sends it to another.
///
/// Handles multi-part messages and labels properly.
///
/// @param receiver Socket to receive message on
/// @param sender Socket to send message to
/// @return bool Whether operation completed without error
bool TransferMessage(::zmq::socket_t &receiver, ::zmq::socket_t &sender);

}} // end namespaces

#endif
