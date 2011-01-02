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

#ifndef ZIPPYLOG_UTIL_HPP_
#define ZIPPYLOG_UTIL_HPP_

#include <zippylog/zippylog.hpp>

#include <string>
#include <vector>

namespace zippylog {
namespace util {

/// Helper function for command argument parsing
///
/// returns true if we found a value. false can be returned in the following scenarios:
///   - bad iterator
///   - no more strings after iterator
///   - next string begins with "-" or "--"
///
/// in all these cases, no arguments are popped
ZIPPYLOG_EXPORT bool GetArgumentValueAndPop(::std::vector< ::std::string > &args, size_t &index, ::std::string &value);

}} // namespaces

#endif