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

#include <zippylog/client.hpp>

#include <gtest/gtest.h>

using ::std::invalid_argument;
using ::std::string;
using ::std::vector;

namespace zippylog {
namespace client {

class ClientTest : public ::testing::Test
{
    protected:

};

TEST_F(ClientTest, StartParamValidation)
{
    ASSERT_TRUE(true);
}

}} // namespaces