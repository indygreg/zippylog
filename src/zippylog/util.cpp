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

#include <zippylog/util.hpp>

namespace zippylog {
namespace util {

using ::std::string;
using ::std::vector;

bool GetArgumentValueAndPop(vector<string> &args, size_t &index, string &value)
{
    if (args.size() - index < 2) return false;

    if (args[index+1].at(0) == '-' || args[index+1].substr(0, 1) == "--") return false;

    value = args[++index];

    args.erase(args.begin() + index - 1);
    args.erase(args.begin() + index - 1);

    index--;

    return true;
}

}} // namespaces