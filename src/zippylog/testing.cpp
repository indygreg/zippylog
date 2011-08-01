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

#include <zippylog/testing.hpp>

#include <zippylog/platform.hpp>

#include <google/protobuf/descriptor.h>

using ::std::string;
using ::std::vector;

using namespace zippylog;

namespace zippylog {
namespace testing {

void TestBase::SetUp()
{
    this->registrar = ::zippylog::MessageRegistrar::instance();
    this->registrar->GetAllEnumerations(this->enumerations);

    ::testing::Test::SetUp();
}

string TestBase::GetTemporaryDirectory()
{
  string id = ::zippylog::platform::CreateUUID(true);
  string path = "test/stores/" + id;

  EXPECT_FALSE(platform::PathIsDirectory(path));
  EXPECT_TRUE(platform::MakeDirectory(path));

  this->created_store_paths.push_back(path);

  return path;
}

string TestBase::GetTemporaryStoreUri()
{
    string path = this->GetTemporaryDirectory();
    string uri = "simpledirectory://" + path;

    return uri;
}

Store * TestBase::GetTemporaryStore()
{
    Store *s = Store::CreateStore(this->GetTemporaryStoreUri());

    this->stores.push_back(s);

    return s;
}

string TestBase::GetTemporaryStreamPath()
{
    string dir = this->GetTemporaryDirectory();
    string uuid = ::zippylog::platform::CreateUUID(true);

    return ::zippylog::platform::PathJoin(dir, uuid + ".zippylog");
}

void TestBase::TearDown()
{
    for (vector<Store *>::size_type i = 0; i < this->stores.size(); i++) {
        delete this->stores[i];
    }
    this->stores.clear();

    vector<string>::iterator i = this->created_store_paths.begin();
    vector<string>::iterator end = this->created_store_paths.end();
    for (; i != end; ++i) {
        EXPECT_TRUE(::zippylog::platform::RemoveDirectory(*i));
    }

    this->created_store_paths.clear();

    ::testing::Test::TearDown();
}

Envelope TestBase::GetRandomEnvelope(uint32 max_messages)
{
    Envelope e;

    uint32 message_count = (rand() % max_messages) + 1;

    while (message_count) {
        this->AddRandomMessage(e);
        --message_count;
    }

    return e;
}

Envelope TestBase::GetRandomEnvelope(uint32 min, uint32 max)
{
    Envelope e;
    uint32 message_count = (rand() % max) + min;

    while (message_count) {
        this->AddRandomMessage(e);
        --message_count;
    }

    return e;
}

void TestBase::AddRandomMessage(Envelope &e)
{
    int32 index = rand() % this->enumerations.size();

    ::std::pair<uint32, uint32> enumeration = this->enumerations[index];

    ::google::protobuf::Message *m = this->GetRandomMessage(enumeration.first, enumeration.second);

    e.AddMessage(*m, enumeration.first, enumeration.second);
    delete m;
}

::google::protobuf::Message * TestBase::GetRandomMessage(uint32 ns, uint32 enumeration)
{
    ::google::protobuf::Message *m = this->registrar->GetMessage(ns, enumeration);
    const ::google::protobuf::Descriptor *d = m->GetDescriptor();
    const ::google::protobuf::Message::Reflection *r = m->GetReflection();

    for (int32 i = 0; i < d->field_count(); i++) {
        const ::google::protobuf::FieldDescriptor *fd = d->field(i);

        bool repeated = fd->is_repeated();
        bool required = fd->is_required();

        // randomly include optional fields
        if (!required && rand() % 2) continue;

        switch (fd->type()) {
            case ::google::protobuf::FieldDescriptor::TYPE_BOOL:
                if (repeated) {
                    r->AddBool(m, fd, true);
                }
                else {
                    r->SetBool(m, fd, true);
                }
                break;

            case ::google::protobuf::FieldDescriptor::TYPE_DOUBLE:
                if (repeated) {
                    r->AddDouble(m, fd, 0.3254);
                }
                else {
                    r->SetDouble(m, fd, 0.3254);
                }
                break;

            case ::google::protobuf::FieldDescriptor::TYPE_FLOAT:
                if (repeated) {
                    r->AddFloat(m, fd, 0.3254);
                }
                else {
                    r->SetFloat(m, fd, 0.3254);
                }
                break;

            case ::google::protobuf::FieldDescriptor::TYPE_FIXED32:
            case ::google::protobuf::FieldDescriptor::TYPE_INT32:
            case ::google::protobuf::FieldDescriptor::TYPE_SFIXED32:
            case ::google::protobuf::FieldDescriptor::TYPE_SINT32:
                if (repeated) {
                    r->AddInt32(m, fd, rand());
                }
                else {
                    r->SetInt32(m, fd, rand());
                }
                break;

            case ::google::protobuf::FieldDescriptor::TYPE_UINT32:
                if (repeated) {
                    r->AddUInt32(m, fd, rand());
                }
                else {
                    r->SetUInt32(m, fd, rand());
                }
                break;

            case ::google::protobuf::FieldDescriptor::TYPE_FIXED64:
            case ::google::protobuf::FieldDescriptor::TYPE_INT64:
            case ::google::protobuf::FieldDescriptor::TYPE_SFIXED64:
            case ::google::protobuf::FieldDescriptor::TYPE_SINT64:
                if (repeated) {
                    r->AddInt64(m, fd, rand());
                }
                else {
                    r->SetInt64(m, fd, rand());
                }
                break;

            case ::google::protobuf::FieldDescriptor::TYPE_UINT64:
                if (repeated) {
                    r->AddUInt64(m, fd, rand());
                }
                else {
                    r->SetUInt64(m, fd, rand());
                }
                break;

            case ::google::protobuf::FieldDescriptor::TYPE_STRING:
            case ::google::protobuf::FieldDescriptor::TYPE_BYTES:
                if (repeated) {
                    r->AddString(m, fd, "foo");
                }
                else {
                    r->SetString(m, fd, "foo");
                }
                break;

            case ::google::protobuf::FieldDescriptor::TYPE_ENUM:
            {
                const ::google::protobuf::EnumDescriptor *ed = fd->enum_type();
                const ::google::protobuf::EnumValueDescriptor *evd = ed->value(rand() % ed->value_count());

                if (repeated) {
                    r->AddEnum(m, fd, evd);
                }
                else {
                    r->SetEnum(m, fd, evd);
                }
            }
                break;

            case ::google::protobuf::FieldDescriptor::TYPE_GROUP:
            case ::google::protobuf::FieldDescriptor::TYPE_MESSAGE:

                break;

        }
    }

    return m;
}

}} // namespace
