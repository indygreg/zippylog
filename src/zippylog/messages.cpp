
// this file is automatically generated. you are highly discouraged from
// editing it manually

#include <zippylog/messages.hpp>
#include <zippylog/message_registrar.hpp>

namespace zippylog {

    void register_known_messages()
    {
        ::zippylog::MessageRegistrar *registrar = ::zippylog::MessageRegistrar::instance();

        registrar->register_message(1, 80, new ::zippylog::request_processor::Create());
        registrar->register_message(1, 81, new ::zippylog::request_processor::Destroy());
        registrar->register_message(1, 82, new ::zippylog::request_processor::RunStart());
        registrar->register_message(1, 83, new ::zippylog::request_processor::RunStop());
        registrar->register_message(1, 84, new ::zippylog::request_processor::FailReceiveMessage());
        registrar->register_message(1, 85, new ::zippylog::request_processor::ReceiveEmptyMessage());
        registrar->register_message(1, 86, new ::zippylog::request_processor::UnknownMessageVersion());
        registrar->register_message(1, 87, new ::zippylog::request_processor::UnknownRequestType());
        registrar->register_message(1, 88, new ::zippylog::request_processor::EnvelopeParseFailure());
        registrar->register_message(1, 89, new ::zippylog::request_processor::EmptyEnvelope());
        registrar->register_message(1, 90, new ::zippylog::request_processor::InvalidMessageEnumeration());
        registrar->register_message(1, 91, new ::zippylog::request_processor::BeginProcessStoreInfo());
        registrar->register_message(1, 92, new ::zippylog::request_processor::EndProcessStoreInfo());
        registrar->register_message(1, 93, new ::zippylog::request_processor::ReceiveInvalidGet());
        registrar->register_message(1, 94, new ::zippylog::request_processor::GetInvalidStream());
        registrar->register_message(1, 95, new ::zippylog::request_processor::GetInvalidOffset());
        registrar->register_message(1, 96, new ::zippylog::request_processor::BeginProcessGet());
        registrar->register_message(1, 97, new ::zippylog::request_processor::EndProcessGet());
        registrar->register_message(1, 98, new ::zippylog::request_processor::SendErrorResponse());
        registrar->register_message(1, 99, new ::zippylog::request_processor::ForwardSubscribeKeepalive());
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
        registrar->register_message(1, 49, new ::zippylog::zippylogd::StreamerStartup());
        registrar->register_message(1, 50, new ::zippylog::zippylogd::StreamerShutdown());
        registrar->register_message(1, 40, new ::zippylog::zippylogd::StreamerSubscriptionExpired());
        registrar->register_message(1, 41, new ::zippylog::zippylogd::StreamerReceiveKeepalive());
        registrar->register_message(1, 42, new ::zippylog::zippylogd::StreamerSubscriptionRenewedFromKeepalive());
        registrar->register_message(1, 43, new ::zippylog::zippylogd::StreamerRejectKeepaliveUnknownSubscription());
        registrar->register_message(1, 44, new ::zippylog::zippylogd::StreamerErrorRenewingSubscription());
        registrar->register_message(1, 51, new ::zippylog::zippylogd::StoreWatcherStartup());
        registrar->register_message(1, 52, new ::zippylog::zippylogd::StoreWatcherShutdown());
        registrar->register_message(1, 55, new ::zippylog::protocol::response::FeatureSpecification());
        registrar->register_message(1, 7, new ::zippylog::protocol::response::StreamSegmentStart());
        registrar->register_message(1, 8, new ::zippylog::protocol::response::StreamSegmentEnd());
        registrar->register_message(1, 24, new ::zippylog::protocol::response::SubscribeAck());
        registrar->register_message(1, 26, new ::zippylog::protocol::response::SubscriptionStart());
        registrar->register_message(1, 9, new ::zippylog::protocol::response::Error());
        registrar->register_message(1, 56, new ::zippylog::protocol::request::GetFeatures());
        registrar->register_message(1, 57, new ::zippylog::protocol::request::GetStoreInfo());
        registrar->register_message(1, 11, new ::zippylog::protocol::request::Get());
        registrar->register_message(1, 58, new ::zippylog::protocol::request::WriteEnvelope());
        registrar->register_message(1, 59, new ::zippylog::protocol::request::WriteEnvelopeWithMessages());
        registrar->register_message(1, 22, new ::zippylog::protocol::request::SubscribeStoreChanges());
        registrar->register_message(1, 54, new ::zippylog::protocol::request::SubscribeEnvelopes());
        registrar->register_message(1, 23, new ::zippylog::protocol::request::SubscribeKeepalive());
        registrar->register_message(1, 25, new ::zippylog::protocol::request::SubscribeCancel());
        // @@protoc_insertion_point(message_registration)
    }
}
