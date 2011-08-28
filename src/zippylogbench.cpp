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

#include <zippylog/zippylog.hpp>
#include <zippylog/testing.hpp>

#include <zippylog/client.hpp>
#include <zippylog/envelope.hpp>
#include <zippylog/lua.hpp>
#include <zippylog/platform.hpp>
#include <zippylog/request_processor.pb.h>
#include <zippylog/store.hpp>
#include <zippylog/device/server.hpp>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <assert.h>

using ::std::cerr;
using ::std::cout;
using ::std::endl;
using ::std::ostringstream;
using ::std::string;
using ::std::vector;
using ::zippylog::client::Client;
using ::zippylog::Envelope;
using ::zippylog::device::ServerStartParams;
using ::zippylog::lua::LuaState;
using ::zippylog::Store;

#define TIMER_START(iterations) { \
    int TIMER_ITERATIONS = iterations; \
    ::zippylog::platform::Time __tstart; \
    ::zippylog::platform::TimeNow(__tstart); \
    for (int TIMER_LOOP_VALUE = TIMER_ITERATIONS; TIMER_LOOP_VALUE; TIMER_LOOP_VALUE--) { \

#define TIMER_START_SINGLE() { \
    ::zippylog::platform::Time __tstart; \
    ::zippylog::platform::TimeNow(__tstart); { \

#define TIMER_END(description) } \
    ::zippylog::platform::Time __tend; \
    ::zippylog::platform::TimeNow(__tend); \
    uint64 elapsed = __tend.epoch_micro - __tstart.epoch_micro; \
    print_result(description, elapsed, TIMER_ITERATIONS); \
    }

#define TIMER_END_COUNT(description, count) } \
    ::zippylog::platform::Time __tend; \
    ::zippylog::platform::TimeNow(__tend); \
    uint64 elapsed = __tend.epoch_micro - __tstart.epoch_micro; \
    print_result(description, elapsed, count); \
    }

// this is like the one above except we take an int whose value
// we wait to go to the number of iterations
#define TIMER_END_WAIT(description, tracking) } \
    while (tracking != TIMER_ITERATIONS); \
    ::zippylog::platform::Time __tend; \
    ::zippylog::platform::TimeNow(__tend); \
    uint64 elapsed = __tend.epoch_micro - __tstart.epoch_micro; \
    print_result(description, elapsed, TIMER_ITERATIONS); \
    }

namespace zippylog {

/// Test fixture for benchmarks
class Benchmark : public ::zippylog::testing::TestBase
{ };

void print_result(string what, uint64 microseconds, uint64 ops)
{
    double seconds = (1.00 * microseconds) / 1000000.00;
    double rate = (1.00 * ops) / seconds;

    cout << what << " " << rate << "/s" << endl;
}

TEST_F(Benchmark, LuaFunctionCalls)
{
    lua_State *L = luaL_newstate();
    const char *function = "function test()\n    return nil\nend";
    assert(!luaL_loadstring(L, function));

    TIMER_START(1000000);
    lua_getglobal(L, "test");
    lua_pcall(L, 0, 1, 0);
    lua_pop(L, 1);
    TIMER_END("lua.pcall_empty_function");
}

void zmq_socket_send_recv(const char *address, int sender_type, int receiver_type, int message_size, int iterations, const char *name)
{
    ::zmq::context_t ctx(1);
    ::zmq::socket_t sender(ctx, sender_type);
    ::zmq::socket_t receiver(ctx, receiver_type);

    sender.bind(address);
    receiver.connect(address);

    TIMER_START(iterations);
    ::zmq::message_t msg(message_size);
    ::zmq::message_t received;

    sender.send(msg, 0);
    receiver.recv(&received, 0);
    TIMER_END(name);
}

TEST_F(Benchmark, ZeroMQ)
{
    TIMER_START(1000000);
    ::zmq::message_t *msg = new ::zmq::message_t();
    delete msg;
    TIMER_END("zmq.empty_message_allocation");

    TIMER_START(1000000);
    ::zmq::message_t *msg = new ::zmq::message_t(100);
    delete msg;
    TIMER_END("zmq.100_byte_message_allocation");

    TIMER_START(1000000);
    ::zmq::message_t *msg = new ::zmq::message_t(1000);
    delete msg;
    TIMER_END("zmq.1000_byte_message_allocation");

    TIMER_START(100000);
    ::zmq::message_t *msg = new ::zmq::message_t(10000);
    delete msg;
    TIMER_END("zmq.10000_byte_message_allocation");

    TIMER_START(10000);
    ::zmq::message_t *msg = new ::zmq::message_t(100000);
    delete msg;
    TIMER_END("zmq.100000_byte_message_allocation");

    zmq_socket_send_recv("inproc://00", ZMQ_PUSH, ZMQ_PULL, 0, 1000000, "zmq.socket.inproc.pushpull.empty");
    zmq_socket_send_recv("inproc://01", ZMQ_PUSH, ZMQ_PULL, 100, 1000000, "zmq.socket.inproc.pushpull.100_byte_message");
    zmq_socket_send_recv("inproc://02", ZMQ_PUSH, ZMQ_PULL, 1000, 1000000, "zmq.socket.inproc.pushpull.1000_byte_message");
    zmq_socket_send_recv("inproc://03", ZMQ_PUSH, ZMQ_PULL, 10000, 1000000, "zmq.socket.inproc.pushpull.10000_byte_message");
    zmq_socket_send_recv("inproc://04", ZMQ_PUSH, ZMQ_PULL, 100000, 100000, "zmq.socket.inproc.pushpull.100000_byte_message");
    zmq_socket_send_recv("inproc://05", ZMQ_PUSH, ZMQ_PULL, 1000000, 100000, "zmq.socket.inproc.pushpull.1000000_byte_message");
}

TEST_F(Benchmark, EnvelopeEmptyAllocations)
{
    TIMER_START(1000000);
    Envelope *e = new Envelope();
    delete e;
    TIMER_END("zippylog.envelope.empty_allocations");
}

TEST_F(Benchmark, EnvelopeEmptySerialize)
{
    Envelope e;
    string s;
    TIMER_START(1000000);
    e.Serialize(s);
    TIMER_END("zippylog.envelope.empty_serialize");
}

TEST_F(Benchmark, EnvelopeEmptyZmqSerialize)
{
    Envelope e;
    ::zmq::message_t msg;
    TIMER_START(1000000);
    e.ToZmqMessage(msg);
    TIMER_END("zippylog.envelope.empty_zmq_serialize");
}

TEST_F(Benchmark, EnvelopeEmptyZmqProtocolSerialize)
{
    Envelope e;
    ::zmq::message_t msg;
    TIMER_START(1000000);
    e.ToProtocolZmqMessage(msg);
    TIMER_END("zippylog.envelope.empty_zmq_protocol_serialize");
}

TEST_F(Benchmark, EnvelopeEmptyConstructFromBuffer)
{
    Envelope empty;
    string serialized;
    empty.Serialize(serialized);
    void * buffer = (void *)serialized.data();
    int length = serialized.length();
    TIMER_START(1000000);
    Envelope e(buffer, length);
    TIMER_END("zippylog.envelope.empty_construct_from_buffer");
}

TEST_F(Benchmark, EnvelopeEmptyConstructFromString)
{
    Envelope empty;
    string serialized;
    empty.Serialize(serialized);
    TIMER_START(1000000);
    Envelope e(serialized);
    TIMER_END("zippylog.envelope.empty_construct_from_string");
}

TEST_F(Benchmark, EnvelopeEmptyConstructFromZmq)
{
    Envelope empty;
    ::zmq::message_t msg;
    empty.ToZmqMessage(msg);
    TIMER_START(1000000);
    Envelope e(msg);
    TIMER_END("zippylog.envelope.empty.construct_from_zmq");
}

TEST_F(Benchmark, EnvelopeEmptyCopyConstructor)
{
    Envelope empty;
    TIMER_START(1000000);
    Envelope e(empty);
    TIMER_END("zippylog.envelope.empty_copy_constructor");
}

TEST_F(Benchmark, EnvelopeAddMessage)
{
    TIMER_START(1000000);
    Envelope e;
    ::zippylog::request_processor::Destroy msg;
    msg.add_to_envelope(e);
    TIMER_END("zippylog.envelope.add_1_message");

    TIMER_START(1000000);
    Envelope e;
    ::zippylog::request_processor::Destroy msg;
    msg.add_to_envelope(e);
    msg.add_to_envelope(e);
    TIMER_END("zippylog.envelope.add_2_messages");

    TIMER_START(1000000);
    Envelope e;
    ::zippylog::request_processor::Destroy msg;
    msg.add_to_envelope(e);
    msg.add_to_envelope(e);
    msg.add_to_envelope(e);
    TIMER_END("zippylog.envelope.add_3_messages");

    TIMER_START(100000);
    Envelope e;
    ::zippylog::request_processor::Destroy msg;
    for (int i = 5; i; --i) {
        msg.add_to_envelope(e);
    }
    TIMER_END("zippylog.envelope.add_5_messages");

    TIMER_START(100000);
    Envelope e;
    ::zippylog::request_processor::Destroy msg;
    for (int i = 10; i; --i) {
        msg.add_to_envelope(e);
    }
    TIMER_END("zippylog.envelope.add_10_messages");

    TIMER_START(10000);
    Envelope e;
    ::zippylog::request_processor::Destroy msg;
    for (int i = 100; i; --i) {
        msg.add_to_envelope(e);
    }
    TIMER_END("zippylog.envelope.add_100_messages");

    TIMER_START(10000);
    Envelope e;
    ::zippylog::request_processor::Destroy msg;
    for (int i = 1000; i; --i) {
        msg.add_to_envelope(e);
    }
    TIMER_END("zippylog.envelope.add_1000_messages");
}

void client_ping_callback(Client *, void *data) {
    uint32 *i = (uint32 *)data;
    (*i)++;
}

/*
TEST_F(Benchmark, Server)
{
    ::zmq::context_t ctx(3);

    ServerStartParams start_params;
    start_params.ctx = &ctx;
    start_params.store_path = "simpledirectory://test/stores/00-simple";
    start_params.listen_endpoints.push_back("inproc://server00");
    start_params.log_bucket.clear();
    start_params.log_stream_set.clear();

    ::zippylog::device::Server server(start_params);
    server.RunAsync();

    {
        ::zmq::socket_t cs(ctx, ZMQ_REQ);
        cs.connect("inproc://server00");

        ::zippylog::protocol::request::PingV1 ping;
        Envelope e;
        ping.add_to_envelope(e);

        TIMER_START(100000);
        ::zmq::message_t msg;
        e.ToProtocolZmqMessage(msg);
        cs.send(msg, NULL);
        ::zmq::message_t response;
        cs.recv(&response, NULL);
        TIMER_END("zippylog.server.ping_request_reply");
    }

    {
        Client client(&ctx, "inproc://server00");
        int count = 0;

        TIMER_START(100000);
        client.Ping(client_ping_callback, (void *)&count);
        TIMER_END_WAIT("zippylog.server.ping_bulk_send", count);
    }

    server.Shutdown();
}
*/

TEST_F(Benchmark, LuaLoadString)
{
    LuaState l;
    string error;
    l.LoadLuaCode("function zippylog_load_string(s)\n"
                  "  e = zippylog.envelope.new()\n"
                  "  e:set_string_value(s)\n"
                  "  return e\n"
                  "end", error);

    ::zippylog::lua::LoadStringResult result;
    const string input = "foo bar 2k";
    TIMER_START(10000);
    l.ExecuteLoadString(input, result);
    TIMER_END("zippylog.lua.load_string_return_simple_envelope");
}

TEST_F(Benchmark, SimpleStoreIO)
{
    {
        const int32 n = 1000000;
        Envelope e;

        Store * s = this->GetTemporaryStore();
        ASSERT_TRUE(s->CreateBucket("foo"));
        ASSERT_TRUE(s->CreateStreamset("foo", "bar"));

        TIMER_START(n);
        s->WriteEnvelope("foo", "bar", e);
        TIMER_END("zippylog.store.write_1M_empty_envelopes");
    }

    {
        const int32 n = 1000000;
        Envelope e;

        string path = ::zippylog::platform::PathJoin(this->GetTemporaryDirectory(), "s.zippylog");

        {
            ::zippylog::FileOutputStream fos(path);
            for (int32 i = 0; i < n; i++) {
                fos.WriteEnvelope(e);
            }
        }

        ::zippylog::FileInputStream fis(path);
        uint32 read;
        TIMER_START(n);
        fis.ReadEnvelope(e, read);
        TIMER_END("zippylog.FileInputStream.read_1M_empty_envelopes");
    }

    {
        const int32 n = 1000000;

        Envelope *envelopes = new Envelope[n];
        for (int32 i = 0; i < n; i++) {
            envelopes[i] = this->GetRandomEnvelope();
        }

        {
            Store * s = this->GetTemporaryStore();
            ASSERT_TRUE(s->CreateBucket("foo"));
            ASSERT_TRUE(s->CreateStreamset("foo", "bar"));

            TIMER_START_SINGLE();
            for (int32 i = 0; i < n; i++) {
                s->WriteEnvelope("foo", "bar", envelopes[i]);
            }
            TIMER_END_COUNT("zippylog.store.write_1M_random_envelopes", n);
        }

        {
            string path = ::zippylog::platform::PathJoin(this->GetTemporaryDirectory(), "s.zippylog");

            {
                ::zippylog::FileOutputStream fos(path);
                TIMER_START(n);
                fos.WriteEnvelope(envelopes[TIMER_LOOP_VALUE - 1]);
                TIMER_END("zippylog.FileOutputStream.write_1M_random_envelopes");
            }

            Envelope e;
            ::zippylog::FileInputStream fis(path);
            uint32 read;
            TIMER_START(n);
            fis.ReadEnvelope(e, read);
            TIMER_END("zippylog.FileInputStream.read_1M_random_envelopes");
        }

        delete [] envelopes;
    }
}

} // namespace

/// Our custom gtest listener for result printing
///
/// It swallows all output except for failures, which should never occur
/// during benchmarking.
class BenchmarkListener : public ::testing::EmptyTestEventListener {
    // Called after a failed assertion or a SUCCEED() invocation.
    virtual void OnTestPartResult(
        const ::testing::TestPartResult& result)
    {
        if (result.failed()) {
            printf("UNEXPECTED FAILURE in %s:%d\n%s\n",
                result.file_name(), result.line_number(), result.summary());
        }
    }
  };

int main(int argc, char **argv)
{
    ::zippylog::initialize_library();

    ::testing::InitGoogleTest(&argc, argv);

    // Use a custom listener for better formatting.
    ::testing::TestEventListeners& listeners =
      ::testing::UnitTest::GetInstance()->listeners();
    delete listeners.Release(listeners.default_result_printer());
    listeners.Append(new BenchmarkListener);

    int result = RUN_ALL_TESTS();

    ::zippylog::shutdown_library();

    return result;
}

