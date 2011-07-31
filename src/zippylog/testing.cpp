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

using ::std::string;
using ::std::vector;

using namespace zippylog;

namespace zippylog {
namespace testing {

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

void TestBase::TearDown()
{
    vector<string>::iterator i = this->created_store_paths.begin();
    vector<string>::iterator end = this->created_store_paths.end();
    for (; i != end; ++i) {
        EXPECT_TRUE(::zippylog::platform::RemoveDirectory(*i));
    }

    this->created_store_paths.clear();
}

}} // namespace
