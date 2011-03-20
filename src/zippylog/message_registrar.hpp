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

#ifndef ZIPPYLOG_MESSAGE_REGISTRAR_HPP_
#define ZIPPYLOG_MESSAGE_REGISTRAR_HPP_

#include <zippylog/zippylog.hpp>

#include <google/protobuf/message.h>
#include <map>
#include <utility>

namespace zippylog {

// a central registration service for zippylog message enumerations
// Currently, instance() is NOT thread safe
class ZIPPYLOG_EXPORT MessageRegistrar {
    public:
        ~MessageRegistrar();

        static MessageRegistrar * instance();
        static void TerminateInstance();

        bool register_message(uint32 ns, uint32 enumeration, ::google::protobuf::Message *instance);

        /// Obtains a new, empty instance of a message of the specified
        /// enumeration.
        ///
        /// The returned message is actually an instance of the message type
        /// requested, just casted as the base protocol buffer message type.
        ///
        /// The memory address returned is owned by the caller. Therefore,
        /// the caller needs to free it when it is done with it. This is one
        /// of the few places in the library where a returned pointer is owned
        /// by the caller.
        ::google::protobuf::Message * GetMessage(uint32 ns, uint32 enumeration);

        // cleans up a registrar instance by removing all registered message types
        void Cleanup();

    protected:
        // TODO lookup time can be improved since map is nlogn
        ::std::map< ::std::pair<uint32, uint32>, ::google::protobuf::Message * > _types;

    private:
        MessageRegistrar();
        MessageRegistrar(const MessageRegistrar & orig);
        MessageRegistrar & operator=(const MessageRegistrar & orig);

        static MessageRegistrar * _instance;
};

} // namespace

#endif
