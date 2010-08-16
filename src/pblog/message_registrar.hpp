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

#ifndef PBLOG_MESSAGE_REGISTRAR_HPP_
#define PBLOG_MESSAGE_REGISTRAR_HPP_

#include <pblog/pblog.h>

#include <google/protobuf/message.h>
#include <map>
#include <utility>

namespace pblog {

using ::google::protobuf::uint32;
using ::google::protobuf::Message;
using ::std::map;
using ::std::pair;

class PBLOG_EXPORT MessageRegistrar {
    public:
        MessageRegistrar();

        static MessageRegistrar * instance();

        bool register_message(uint32 ns, uint32 enumeration, Message *instance);
        Message * get_message(uint32 ns, uint32 enumeration);

    protected:
        // TODO lookup time can be improved since map is nlogn
        map<pair<uint32, uint32>, Message *> _types;
};

} // namespace

#endif