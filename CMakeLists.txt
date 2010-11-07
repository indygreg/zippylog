#  Copyright 2010 Gregory Szorc
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.

cmake_minimum_required(VERSION 2.6)

project(zippylog)

set(zippylog_VERSION_MAJOR 0)
set(zippylog_VERSION_MINOR 1)

set(CMAKE_THREAD_PREFER_PTHREADS)
set(CMAKE_BUILD_TYPE Debug)

find_package(Protobuf REQUIRED)
find_package(Lua51 REQUIRED)
find_package(Threads REQUIRED)

#configure_file("src/zippylog/config.h.in" "src/zippylog/config.h")

include_directories(src)

include_directories(${LUA_INCLUDE_DIR})

# TODO should find ZeroMQ properly
include_directories(external/zeromq/include)
find_library(ZEROMQ_LIBRARY zmq external/zeromq)

set(lib_sources
    src/zippylog/broker.cpp
    #src/zippylog/client.cpp
    src/zippylog/envelope.cpp
    src/zippylog/lua.cpp
    src/zippylog/message.pb.cc
    src/zippylog/message_registrar.cpp
    src/zippylog/messages.cpp
    src/zippylog/platform.cpp
    src/zippylog/protocol.pb.cc
    src/zippylog/protocol/request.pb.cc
    src/zippylog/protocol/response.pb.cc
    src/zippylog/request_processor.cpp
    src/zippylog/store.cpp
    src/zippylog/store_watcher.cpp
    src/zippylog/stream.cpp
    src/zippylog/streamer.cpp
    src/zippylog/zeromq.cpp
    src/zippylog/zippylogd.pb.cc
    src/zippylog/zippylog.cpp
)

add_library(zippylog SHARED ${lib_sources})
target_link_libraries(zippylog ${LUA_LIBRARIES} ${PROTOBUF_LIBRARY} ${ZEROMQ_LIBRARY} rt)

add_executable(zippylogd src/zippylogd.cpp)
target_link_libraries(zippylogd zippylog)

#if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
add_definitions(-DLINUX)
#endif()

add_definitions(-D_DEBUG)

