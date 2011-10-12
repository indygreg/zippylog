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
#include <vector>

namespace zippylog {

/// Registration service for zippylog message types
///
/// The MessageRegistrar can be thought of as a factory for the strongly
/// enumerated zippylog messages. When the zippylog library loads, it
/// registers each known message type with the registrar. The registrar is
/// later used to obtain the message-specific instances of a class from the
/// zippylog message enumeration.
///
/// The MessageRegistrar is exposed as a singleton service to the library.
/// The initialization and destruction routines are not guaranteed to be
/// thread safe. Therefore, callers should create the singleton instance while
/// they can guarantee only 1 thread will be doing this at once. After the
/// singleton is created, multiple threads can access it at once. The same
/// behavior goes for making changes, as well. Only one thread should register
/// messages at a time or things could blow up.
class ZIPPYLOG_EXPORT MessageRegistrar {
public:
    ~MessageRegistrar();

    /// Obtain the global instance of the registrar.
    ///
    /// This is not thread safe. See class documentation for usage.
    ///
    /// @return Pointer to the global instance
    static MessageRegistrar * instance();

    /// Destroy the singleton instance of the registrar.
    static void TerminateInstance();

    /// Register a message type with the registrar.
    ///
    /// This should be called once for each known zippylog message type.
    ///
    /// @param ns Namespace to register message under
    /// @param enumeration Assigned message number within namespace
    /// @param instance protocol buffer message instance representing message
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
    ///
    /// @param ns Namespace where message is registered
    /// @param enumeration Message type within the namespace
    /// @return Empty message instance
    ::google::protobuf::Message * GetMessage(uint32 ns, uint32 enumeration);

    /// Cleans up a registrar instance.
    ///
    /// Will remove all registered message types. This is typically called
    /// automatically.
    void Cleanup();

    /// Obtains a list of all defined enumerations.
    ///
    /// @param list Container that will hold result after return
    void GetAllEnumerations(::std::vector< ::std::pair<uint32, uint32> > &list);

protected:
    /// Container holding all registered messages
    ///
    /// @todo use more efficient storage (likely array)
    ::std::map< ::std::pair<uint32, uint32>, ::google::protobuf::Message * > _types;

private:
    MessageRegistrar();
    MessageRegistrar(MessageRegistrar const & orig);
    MessageRegistrar & operator=(MessageRegistrar const & orig);

    static MessageRegistrar * _instance;
};

} // namespace

#endif
