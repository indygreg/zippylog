
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
        registrar->register_message(1, 53, new ::zippylog::zippylogd::BrokerFlushOutputStreams());
        registrar->register_message(1, 45, new ::zippylog::zippylogd::WorkerStartup());
        registrar->register_message(1, 46, new ::zippylog::zippylogd::WorkerShutdown());
        registrar->register_message(1, 27, new ::zippylog::zippylogd::WorkerFailReceiveMessage());
        registrar->register_message(1, 28, new ::zippylog::zippylogd::WorkerReceiveEmptyMessage());
        registrar->register_message(1, 47, new ::zippylog::zippylogd::WorkerReceiveUnknownRequestType());
        registrar->register_message(1, 29, new ::zippylog::zippylogd::WorkerRequestParseFailure());
        registrar->register_message(1, 30, new ::zippylog::zippylogd::WorkerRequestEmptyEnvelope());
        registrar->register_message(1, 31, new ::zippylog::zippylogd::WorkerInvalidMessageEnumeration());
        registrar->register_message(1, 32, new ::zippylog::zippylogd::WorkerBeginProcessStoreInfo());
        registrar->register_message(1, 33, new ::zippylog::zippylogd::WorkerEndProcessStoreInfo());
        registrar->register_message(1, 34, new ::zippylog::zippylogd::WorkerReceiveInvalidGet());
        registrar->register_message(1, 35, new ::zippylog::zippylogd::WorkerGetInvalidStream());
        registrar->register_message(1, 36, new ::zippylog::zippylogd::WorkerGetInvalidOffset());
        registrar->register_message(1, 37, new ::zippylog::zippylogd::WorkerBeginProcessGet());
        registrar->register_message(1, 38, new ::zippylog::zippylogd::WorkerEndProcessGet());
        registrar->register_message(1, 39, new ::zippylog::zippylogd::WorkerSendErrorResponse());
        registrar->register_message(1, 48, new ::zippylog::zippylogd::WorkerForwardSubscribeKeepalive());
        registrar->register_message(1, 49, new ::zippylog::zippylogd::StreamerStartup());
        registrar->register_message(1, 50, new ::zippylog::zippylogd::StreamerShutdown());
        registrar->register_message(1, 40, new ::zippylog::zippylogd::StreamerSubscriptionExpired());
        registrar->register_message(1, 41, new ::zippylog::zippylogd::StreamerReceiveKeepalive());
        registrar->register_message(1, 42, new ::zippylog::zippylogd::StreamerSubscriptionRenewedFromKeepalive());
        registrar->register_message(1, 43, new ::zippylog::zippylogd::StreamerRejectKeepaliveUnknownSubscription());
        registrar->register_message(1, 44, new ::zippylog::zippylogd::StreamerErrorRenewingSubscription());
        registrar->register_message(1, 51, new ::zippylog::zippylogd::StoreWatcherStartup());
        registrar->register_message(1, 52, new ::zippylog::zippylogd::StoreWatcherShutdown());
        registrar->register_message(1, 7, new ::zippylog::protocol::response::StreamSegmentStart());
        registrar->register_message(1, 8, new ::zippylog::protocol::response::StreamSegmentEnd());
        registrar->register_message(1, 24, new ::zippylog::protocol::response::SubscribeAck());
        registrar->register_message(1, 26, new ::zippylog::protocol::response::SubscriptionStart());
        registrar->register_message(1, 9, new ::zippylog::protocol::response::Error());
        registrar->register_message(1, 10, new ::zippylog::protocol::request::StoreInfo());
        registrar->register_message(1, 11, new ::zippylog::protocol::request::Get());
        registrar->register_message(1, 22, new ::zippylog::protocol::request::SubscribeStoreChanges());
        registrar->register_message(1, 54, new ::zippylog::protocol::request::SubscribeEnvelopes());
        registrar->register_message(1, 23, new ::zippylog::protocol::request::SubscribeKeepalive());
        registrar->register_message(1, 25, new ::zippylog::protocol::request::SubscribeCancel());
        // @@protoc_insertion_point(message_registration)
    }
}
