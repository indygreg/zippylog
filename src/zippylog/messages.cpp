
// this file is automatically generated. you are highly discouraged from
// editing it manually

#include <zippylog/messages.hpp>
#include <zippylog/message_registrar.hpp>

namespace zippylog {

    void register_known_messages()
    {
        ::zippylog::MessageRegistrar *registrar = ::zippylog::MessageRegistrar::instance();

        registrar->register_message(1, 1, new ::zippylog::protocol::StreamInfo());
        registrar->register_message(1, 2, new ::zippylog::protocol::StreamSetInfo());
        registrar->register_message(1, 3, new ::zippylog::protocol::BucketInfo());
        registrar->register_message(1, 4, new ::zippylog::protocol::StoreInfo());
        registrar->register_message(1, 15, new ::zippylog::protocol::StoreChangeStreamAdded());
        registrar->register_message(1, 16, new ::zippylog::protocol::StoreChangeStreamDeleted());
        registrar->register_message(1, 17, new ::zippylog::protocol::StoreChangeStreamAppended());
        registrar->register_message(1, 18, new ::zippylog::protocol::StoreChangeBucketAdded());
        registrar->register_message(1, 19, new ::zippylog::protocol::StoreChangeBucketDeleted());
        registrar->register_message(1, 20, new ::zippylog::protocol::StoreChangeStreamSetAdded());
        registrar->register_message(1, 21, new ::zippylog::protocol::StoreChangeStreamSetDeleted());
        registrar->register_message(1, 12, new ::zippylog::zippylogd::BrokerStartup());
        registrar->register_message(1, 13, new ::zippylog::zippylogd::BrokerShutdown());
        registrar->register_message(1, 14, new ::zippylog::zippylogd::BrokerReceiveClientMessage());
        registrar->register_message(1, 7, new ::zippylog::protocol::response::StreamSegmentStart());
        registrar->register_message(1, 8, new ::zippylog::protocol::response::StreamSegmentEnd());
        registrar->register_message(1, 9, new ::zippylog::protocol::response::Error());
        registrar->register_message(1, 10, new ::zippylog::protocol::request::StoreInfo());
        registrar->register_message(1, 11, new ::zippylog::protocol::request::Get());
        // @@protoc_insertion_point(message_registration)
    }
}
