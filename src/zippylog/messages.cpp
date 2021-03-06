
// this file is automatically generated. you are highly discouraged from
// editing it manually

#include <zippylog/messages.hpp>
#include <zippylog/message_registrar.hpp>

namespace zippylog {

    void register_known_messages()
    {
        ::zippylog::MessageRegistrar *registrar = ::zippylog::MessageRegistrar::instance();

        registrar->register_message(1, 1, new ::zippylog::request_processor::Create());
        registrar->register_message(1, 2, new ::zippylog::request_processor::Destroy());
        registrar->register_message(1, 3, new ::zippylog::request_processor::RunStart());
        registrar->register_message(1, 4, new ::zippylog::request_processor::RunStop());
        registrar->register_message(1, 5, new ::zippylog::request_processor::FailReceiveMessage());
        registrar->register_message(1, 6, new ::zippylog::request_processor::ReceiveEmptyMessage());
        registrar->register_message(1, 7, new ::zippylog::request_processor::UnknownMessageVersion());
        registrar->register_message(1, 8, new ::zippylog::request_processor::UnknownRequestType());
        registrar->register_message(1, 9, new ::zippylog::request_processor::EnvelopeParseFailure());
        registrar->register_message(1, 10, new ::zippylog::request_processor::EmptyEnvelope());
        registrar->register_message(1, 11, new ::zippylog::request_processor::InvalidMessageEnumeration());
        registrar->register_message(1, 12, new ::zippylog::request_processor::EnvelopeEmbeddedMessageParseFailure());
        registrar->register_message(1, 13, new ::zippylog::request_processor::BeginProcessStoreInfo());
        registrar->register_message(1, 14, new ::zippylog::request_processor::EndProcessStoreInfo());
        registrar->register_message(1, 15, new ::zippylog::request_processor::ReceiveInvalidGet());
        registrar->register_message(1, 16, new ::zippylog::request_processor::GetInvalidStream());
        registrar->register_message(1, 17, new ::zippylog::request_processor::GetInvalidOffset());
        registrar->register_message(1, 18, new ::zippylog::request_processor::BeginProcessGetStream());
        registrar->register_message(1, 19, new ::zippylog::request_processor::EndProcessGetStream());
        registrar->register_message(1, 20, new ::zippylog::request_processor::SendErrorResponse());
        registrar->register_message(1, 21, new ::zippylog::request_processor::ForwardSubscribeKeepalive());
        registrar->register_message(1, 22, new ::zippylog::request_processor::BeginProcessBucketInfo());
        registrar->register_message(1, 23, new ::zippylog::request_processor::EndProcessBucketInfo());
        registrar->register_message(1, 24, new ::zippylog::request_processor::BeginProcessStreamSetInfo());
        registrar->register_message(1, 25, new ::zippylog::request_processor::EndProcessStreamSetInfo());
        registrar->register_message(1, 26, new ::zippylog::request_processor::BeginProcessStreamInfo());
        registrar->register_message(1, 27, new ::zippylog::request_processor::EndProcessStreamInfo());
        registrar->register_message(1, 28, new ::zippylog::request_processor::BeginProcessWriteEnvelope());
        registrar->register_message(1, 29, new ::zippylog::request_processor::EndProcessWriteEnvelope());
        registrar->register_message(1, 93, new ::zippylog::protocol::StreamInfoV1());
        registrar->register_message(1, 94, new ::zippylog::protocol::StreamSetInfoV1());
        registrar->register_message(1, 95, new ::zippylog::protocol::BucketInfoV1());
        registrar->register_message(1, 96, new ::zippylog::protocol::StoreInfoV1());
        registrar->register_message(1, 97, new ::zippylog::protocol::StoreChangeStreamAddedV1());
        registrar->register_message(1, 98, new ::zippylog::protocol::StoreChangeStreamDeletedV1());
        registrar->register_message(1, 99, new ::zippylog::protocol::StoreChangeStreamAppendedV1());
        registrar->register_message(1, 100, new ::zippylog::protocol::StoreChangeBucketAddedV1());
        registrar->register_message(1, 101, new ::zippylog::protocol::StoreChangeBucketDeletedV1());
        registrar->register_message(1, 102, new ::zippylog::protocol::StoreChangeStreamSetAddedV1());
        registrar->register_message(1, 103, new ::zippylog::protocol::StoreChangeStreamSetDeletedV1());
        registrar->register_message(1, 113, new ::zippylog::protocol::PluginStateV1());
        registrar->register_message(1, 127, new ::zippylog::device::store_watcher::Create());
        registrar->register_message(1, 128, new ::zippylog::device::store_watcher::Destroy());
        registrar->register_message(1, 129, new ::zippylog::device::store_watcher::RunStart());
        registrar->register_message(1, 130, new ::zippylog::device::store_watcher::RunFinish());
        registrar->register_message(1, 119, new ::zippylog::device::server::SubscriptionRecord());
        registrar->register_message(1, 120, new ::zippylog::device::server::Create());
        registrar->register_message(1, 121, new ::zippylog::device::server::Destroy());
        registrar->register_message(1, 122, new ::zippylog::device::server::RunStart());
        registrar->register_message(1, 123, new ::zippylog::device::server::RunFinish());
        registrar->register_message(1, 124, new ::zippylog::device::server::FlushOutputStreams());
        registrar->register_message(1, 125, new ::zippylog::device::server::ReceiveClientMessage());
        registrar->register_message(1, 126, new ::zippylog::device::server::SendClientMessage());
        registrar->register_message(1, 70, new ::zippylog::protocol::response::PongV1());
        registrar->register_message(1, 71, new ::zippylog::protocol::response::FeatureSpecificationV1());
        registrar->register_message(1, 104, new ::zippylog::protocol::response::StreamSegmentStartV1());
        registrar->register_message(1, 105, new ::zippylog::protocol::response::StreamSegmentEndV1());
        registrar->register_message(1, 109, new ::zippylog::protocol::response::SubscriptionAcceptAckV1());
        registrar->register_message(1, 110, new ::zippylog::protocol::response::SubscriptionKeepaliveAckV1());
        registrar->register_message(1, 107, new ::zippylog::protocol::response::SubscriptionStartV1());
        registrar->register_message(1, 108, new ::zippylog::protocol::response::WriteAckV1());
        registrar->register_message(1, 114, new ::zippylog::protocol::response::PluginRegisterAckV1());
        registrar->register_message(1, 117, new ::zippylog::protocol::response::PluginUnregisterAckV1());
        registrar->register_message(1, 115, new ::zippylog::protocol::response::PluginStatusV1());
        registrar->register_message(1, 69, new ::zippylog::protocol::response::Error());
        registrar->register_message(1, 91, new ::zippylog::protocol::request::PingV1());
        registrar->register_message(1, 72, new ::zippylog::protocol::request::GetFeaturesV1());
        registrar->register_message(1, 73, new ::zippylog::protocol::request::GetStoreInfoV1());
        registrar->register_message(1, 74, new ::zippylog::protocol::request::GetBucketInfoV1());
        registrar->register_message(1, 75, new ::zippylog::protocol::request::GetStreamSetInfoV1());
        registrar->register_message(1, 76, new ::zippylog::protocol::request::GetStreamInfoV1());
        registrar->register_message(1, 92, new ::zippylog::protocol::request::GetStreamSegmentV1());
        registrar->register_message(1, 78, new ::zippylog::protocol::request::WriteEnvelopeV1());
        registrar->register_message(1, 79, new ::zippylog::protocol::request::WriteEnvelopeFromMessagesV1());
        registrar->register_message(1, 131, new ::zippylog::protocol::request::WriteEnvelopeFromStringV1());
        registrar->register_message(1, 80, new ::zippylog::protocol::request::SubscribeStoreChangesV1());
        registrar->register_message(1, 81, new ::zippylog::protocol::request::SubscribeEnvelopesV1());
        registrar->register_message(1, 82, new ::zippylog::protocol::request::SubscribeKeepaliveV1());
        registrar->register_message(1, 83, new ::zippylog::protocol::request::SubscribeCancelV1());
        registrar->register_message(1, 111, new ::zippylog::protocol::request::RegisterPluginV1());
        registrar->register_message(1, 112, new ::zippylog::protocol::request::UnregisterPluginV1());
        registrar->register_message(1, 116, new ::zippylog::protocol::request::GetPluginStatusV1());
        // @@protoc_insertion_point(message_registration)
    }
}
