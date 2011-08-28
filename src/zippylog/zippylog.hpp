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

#ifndef ZIPPYLOG_HPP_
#define ZIPPYLOG_HPP_

// windows requires a special macro to export symbols from libraries
// we use the token ZIPPYLOG_IMPORT for this
// if we are on Windows, we export symbols by default.
// If ZIPPYLOG_IMPORT is defined, we tell the compiler we're looking for
// external symbols.
// Else, we're not on Windows and the keyword does nothing.
// @todo utilize GCC's visibility attribute (http://gcc.gnu.org/wiki/Visibility)
#ifdef WINDOWS
#if defined(ZIPPYLOG_IMPORT)
#define ZIPPYLOG_EXPORT __declspec(dllimport)
#else
#define ZIPPYLOG_EXPORT __declspec(dllexport)
#endif
#else
#define ZIPPYLOG_EXPORT
#endif

/* define 32 and 64 bit integer types */
// @todo these should be namespaced
#ifdef WINDOWS
typedef __int32 int32;
typedef __int64 int64;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;
#elif HAVE_STDINT
#include <stdint.h>
typedef int32_t int32;
typedef int64_t int64;
typedef uint32_t uint32;
typedef uint64_t uint64;
#else
#error "Integer types not defined on this platform"
#endif

#include <stdexcept>
#include <string>

namespace zippylog {

/// The base zippylog exception
///
/// Zippylog code is expected to throw an exception derived from this for all
/// run-time errors. Zippylog code is allowed to throw exceptions derived from
/// std::logic_error if it makes sense (e.g. obvious coding error)
class ZIPPYLOG_EXPORT Exception : public ::std::runtime_error
{
public:
    /// Construct an exception from a string message
    ///
    /// @param msg Error message
    Exception(::std::string const &msg) : ::std::runtime_error(msg) { }
};

/// An error when deserializing a message
class ZIPPYLOG_EXPORT DeserializeException : public ::std::runtime_error
{
public:
    /// Construct an exception
    DeserializeException() : runtime_error("could not deserialize message") { }
};

/// Initializes the zippylog system
///
/// This should be called once at the beginning of every program/library that
/// uses zippylog.
///
/// The function is not thread safe.
ZIPPYLOG_EXPORT void initialize_library();

/// Runs zippylog shutdown procedures
/// This should be executed before process exit or when zippylog is no longer
/// needed by a process.
///
/// It isn't required to call this function. However, not doing so will result
/// in leaked memory (if the process lingers around). However, if the process
/// is terminating, it is safe to not call this function.
///
/// This function is not thread safe.
ZIPPYLOG_EXPORT void shutdown_library();

static const uint32 message_namespace = 1;

/// Default number of persisted state reactor threads for the server device
static const uint32 server_default_persisted_state_reactor_threads = 1;

/// Default number of worker threads for the server device
static const uint32 server_default_worker_threads = 2;

/// Default subscription TTL for server device (in milliseconds)
static const uint32 server_default_subscription_ttl = 60000;

/// Default stream flush interval for server device (in milliseconds)
static const uint32 server_default_stream_flush_interval = 5000;

/// Default bucket to log internal events to in server device
static const ::std::string server_default_log_bucket = "zippylog";

/// Default stream set to log internal events to in server device
static const ::std::string server_default_log_stream_set = "server";

/// Default value for whether to allow remote-supplied Lua code in server device
static const uint32 server_default_lua_allow = false;

/// Default maximum Lua allocation limit for streaming states, in bytes
static const uint32 server_default_lua_streaming_max_memory = 524288;

static const uint32 lua_default_subscription_memory_ceiling = 524288;

static const uint32 lua_default_plugin_memory_ceiling = 524288;

} // namespace

#endif
