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

#ifndef ZIPPYLOG_DEVICE_STRING_WRITER_HPP_
#define ZIPPYLOG_DEVICE_STRING_WRITER_HPP_

#include <zippylog/zippylog.hpp>
#include <zippylog/store.hpp>
#include <zippylog/device/string_receiver.hpp>

#include <istream>

namespace zippylog {
namespace device {

/// Configurable options for string writer device instances
///
/// See the documentation for ::zippylog::device::StringWriter
class ZIPPYLOG_EXPORT StringWriterStartParams {
public:
    StringWriterStartParams() :
      receiver(NULL),
      output_stream(NULL)
    { }

    ::zippylog::device::StringReceiver *receiver;

    /// Optional ostream reference to send output to
    ::std::ostream *output_stream;

    /// Path to store to open and write to
    ::std::string store_path;

    /// If writing to a store, the default bucket to write to
    ::std::string default_bucket;

    /// If writing to a store, the default stream set to write to
    ::std::string default_stream_set;
};

/// Reads strings from a stream and writes data to a store
///
/// A StringWriter receives strings from a stream, processes them, then
/// writes them to an output pipe, file, or store.
class ZIPPYLOG_EXPORT StringWriter {
public:
    /// Construct a string writer from parameters
    ///
    /// String writers can write to multiple destinations. To control where to
    /// write, see the documentation for StringWriterStartParams.
    StringWriter(StringWriterStartParams &params);

    ~StringWriter();

    /// Run the string writer
    ///
    /// This blocks until one of the following conditions is met:
    ///  - the boolean pointed to by the parameter turns to false
    ///  - the stream being read from encounters an error or EOF
    void Run(bool *active);

protected:
    ::std::ostream *ostream;
    ::zippylog::Store *store;
    ::std::string default_bucket;
    ::std::string default_set;
    ::zippylog::device::StringReceiver *receiver;

private:
    StringWriter(const StringWriter &orig);
    StringWriter & operator=(const StringWriter &orig);
};

}} // namespaces

#endif