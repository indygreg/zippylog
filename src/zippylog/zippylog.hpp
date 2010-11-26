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

#ifndef ZIPPYLOG_HPP_
#define ZIPPYLOG_HPP_

// windows requires a special macro to export symbols from libraries
// we use the token ZIPPYLOG_IMPORT for this
// if we are on Windows, we export symbols by default.
// If ZIPPYLOG_IMPORT is defined, we tell the compiler we're looking for
// external symbols.
// Else, we're not on Windows and the keyword does nothing.
// TODO utilize GCC's visibility attribute (http://gcc.gnu.org/wiki/Visibility)
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
// TODO these should be namespaced
#ifdef WINDOWS
typedef __int32 int32;
typedef __int64 int64;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;
#elif LINUX
#include <stdint.h>
typedef int32_t int32;
typedef int64_t int64;
typedef uint32_t uint32;
typedef uint64_t uint64;
#endif

#include <stdexcept>

namespace zippylog {

/// An error when deserializing a message
class DeserializeException : public ::std::runtime_error
{
    public:
        DeserializeException() : runtime_error("could not deserialize message") { }
};

// initializes the zippylog system
// this should be called at the beginning of every program/library that uses zippylog
ZIPPYLOG_EXPORT void initialize_library();

// runs zippylog shutdown procedures
// This should be executed before process exit or when zippylog is no longer
// needed by a process.
//
// It isn't required to call this function. However, not doing so will result
// in leaked memory.
ZIPPYLOG_EXPORT void shutdown_library();

static const uint32 message_namespace = 1;

} // namespace

#endif