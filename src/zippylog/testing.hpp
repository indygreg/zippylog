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

#ifndef ZIPPYLOG_TESTING_HPP
#define ZIPPYLOG_TESTING_HPP_

#include <gtest/gtest.h>

#include <string>
#include <vector>

namespace zippylog {
namespace testing {

/// Base test class that provides numerous helpful routines
class TestBase : public ::testing::Test {
public:
    /// Obtains a URI to a temporary store
    ///
    /// Every time this is called, a new store is generated. The store is
    /// cleaned up automatically at the end of the test
    ::std::string GetTemporaryStoreUri();

protected:
    virtual void TearDown();

private:
    ::std::vector< ::std::string > created_store_paths;
};

}} // namespaces

#endif // file define