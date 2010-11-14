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

#ifndef ZIPPYLOG_ZIPPYLOGD_UTIL_HPP_
#define ZIPPYLOG_ZIPPYLOGD_UTIL_HPP_

#include <zippylog/zippylog.hpp>
#include <zippylog/device/store_writer.hpp>

#include <zmq.hpp>

#include <string>
#include <vector>

namespace zippylog {
namespace zippylogd {

class ZIPPYLOG_EXPORT ZippylogdStartParams {
public:
    ZippylogdStartParams() :
        mode_server(false),
        zctx(NULL),
        store_writer_envelope_pull_endpoint("inproc://store_writer_envelope_pull")
    { }

    // whether to run a server
    bool mode_server;

    // if running a server, where its config file is
    ::std::string server_config_file;

    // if outputting to a store this is the path to the store
    ::std::string store_path;

    // 0MQ context to use
    //
    // If one is not provided and 0MQ is needed, a new context will be created
    ::zmq::context_t *zctx;

    /// 0MQ endpoint to which to bind the store writer envelope PULL socket
    ///
    /// If not defined, a constant default will be used. This could pose
    /// problems if multiple instances use the default.
    ::std::string store_writer_envelope_pull_endpoint;

};

/// An instance of zippylogd
///
/// zippylogd is a rather complex beast. it can have a number of modes of
/// operation. It might be best to read up on ZippylogdStartParams to see
/// how you can launch zippylogd instances.
class ZIPPYLOG_EXPORT Zippylogd {
public:
    /// Construct a new zippylogd instance from parameters
    ///
    /// Constructor just sets up zippylogd for execution. It doesn't
    /// actually start processing anything.
    Zippylogd(ZippylogdStartParams &params);

    ~Zippylogd();

    /// Runs the zippylogd instance
    ///
    /// Function will block until ...
    bool Run();

protected:
    bool run_server;
    ::std::string server_config_file;

    ::zippylog::device::StoreWriter *writer;

    ::zmq::context_t *ctx;
    bool own_context;

    ::std::string store_path;

private:
    Zippylogd(const Zippylogd &orig);
    Zippylogd & operator=(const Zippylogd &orig);
};

/// Parses zippylogd command arguments
///
/// The parsed parameters are stored in the passed StartParams class. If an
/// error was encountered, the function returns false and the passed string
/// contains an error message suitable for display to the invoker.
///
/// The passed list is likely modified during execution.
ZIPPYLOG_EXPORT bool ParseCommandArguments(::std::vector< ::std::string > &args, ZippylogdStartParams &params, ::std::string & error);

}} // namespaces

#endif