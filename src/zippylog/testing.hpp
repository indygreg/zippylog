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

#ifndef ZIPPYLOG_TESTING_HPP
#define ZIPPYLOG_TESTING_HPP_

#include <gtest/gtest.h>

#include <zippylog/envelope.hpp>
#include <zippylog/message_registrar.hpp>
#include <zippylog/store.hpp>
#include <zippylog/stream.hpp>

#include <zmq.hpp>

#include <string>
#include <utility>
#include <vector>

namespace zippylog {
namespace testing {

/// Base test class that provides numerous helpful routines
class TestBase : public ::testing::Test {
public:
    virtual void SetUp();

    /// Obtains a URI to a temporary store
    ///
    /// Every time this is called, a new store is generated. The store is
    /// cleaned up automatically at the end of the test
    ::std::string GetTemporaryStoreUri();

    /// Obtains a path to a temporary directory
    ///
    /// The directory will exist for the duration of the test and will be
    /// deleted after test execution.
    ::std::string GetTemporaryDirectory();

    /// Obtains a path to a temporary stream
    ///
    /// Does not create the stream. Path should be valid to plug into
    /// a FileOutputStream and then a FileInputStream.
    ::std::string GetTemporaryStreamPath();

    /// Obtains a temporary store.
    ///
    /// The store is cleaned up automatically when the test is torn down.
    ::zippylog::Store * GetTemporaryStore();

    /// Obtains an envelope containing random messages
    ::zippylog::Envelope GetRandomEnvelope(uint32 max_messages = 10);

    /// Obtains an envelope containing a number of messages between bounds
    ::zippylog::Envelope GetRandomEnvelope(uint32 min_messages, uint32 max_messages);

    /// Obtains a random message of the specified namespace and enumeration
    ///
    /// The memory backing the message is owned by the caller. This is unlike
    /// many other test APIs, which clean up after themselves.
    ::google::protobuf::Message * GetRandomMessage(uint32 ns, uint32 enumeration);

    /// Adds a random message to the specified envelope
    void AddRandomMessage(::zippylog::Envelope &e);

    /// Obtains a 0MQ push-pull socket pair
    ///
    /// The first socket is the push. The second is the pull socket.
    ::std::pair< ::zmq::socket_t *, ::zmq::socket_t * > GetPushPullSocketPair();

protected:
    virtual void TearDown();

private:
    ::std::vector< ::std::string > created_store_paths;
    ::std::vector< ::zippylog::Store * > stores;

    ::std::vector< ::std::pair<uint32, uint32> > enumerations;

    ::zippylog::MessageRegistrar * registrar;

    ::zmq::context_t *ctx;
    ::std::vector< ::zmq::socket_t * > sockets;
};

}} // namespaces

#endif // file define
