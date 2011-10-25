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

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(FATAL_ERROR "Building with CMake is not currently supported on Windows. See the Windows development documentation in the doc directory")
endif()

set(CMAKE_THREAD_PREFER_PTHREADS)
set(CMAKE_BUILD_TYPE Debug)

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 --coverage")
endif(CMAKE_COMPILER_IS_GNUCXX)


#set(CMAKE_VERBOSE_MAKEFILE ON)

################
# DEPENDENCIES #
################

# Google C++ Testing Framework
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Protocol Buffers
find_package(Protobuf REQUIRED)

# Lua
find_package(Lua51 REQUIRED)
include_directories(${LUA_INCLUDE_DIR})

# ZeroMQ
# TODO should find ZeroMQ properly
include_directories(external/zeromq/include external/cppzmq)
find_library(ZEROMQ_LIBRARY zmq HINTS external/zeromq/src/.libs)

# Misc
find_package(Threads REQUIRED)


##############################
# Global Build Configuration #
##############################

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
add_definitions(-DLINUX)
add_definitions(-DPOSIX)
add_definitions(-DHAVE_STDINT)
add_definitions(-DHAVE_PTHREAD)
add_definitions(-DHAVE_OPENDIR)
add_definitions(-DHAVE_INOTIFY)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
add_definitions(-DMACOS)
add_definitions(-DPOSIX)
add_definitions(-DHAVE_STDINT)
add_definitions(-DHAVE_PTHREAD)
add_definitions(-DHAVE_OPENDIR)
else()
    message(FATAL_ERROR "Your system type is not supported at this time.")
endif()

# add our source directory
include_directories(BEFORE src)

#############################
# libzippylog configuration #
#############################
set(libzippylog_sources
    src/zippylog/platform.cpp # first b/c most likely to blow up compiler
    src/zippylog/client.cpp
    src/zippylog/core.pb.cc
    src/zippylog/device/device.cpp
    src/zippylog/device/persisted_state_reactor.cpp
    src/zippylog/device/server.cpp
    src/zippylog/device/server.pb.cc
    src/zippylog/device/server.pb-lua.cc
    src/zippylog/device/server_watcher.cpp
    src/zippylog/device/server_worker.cpp
    src/zippylog/device/store_watcher.pb.cc
    src/zippylog/device/store_watcher.pb-lua.cc
    src/zippylog/device/store_writer.cpp
    src/zippylog/device/store_writer_sender.cpp
    src/zippylog/device/string_loader.cpp
    src/zippylog/envelope.cpp
    src/zippylog/lua.cpp
    src/zippylog/message.pb.cc
    src/zippylog/message.pb-lua.cc
    src/zippylog/message_registrar.cpp
    src/zippylog/messages.cpp
    src/zippylog/persisted_state_manager.cpp
    src/zippylog/protocol.pb.cc
    src/zippylog/protocol.pb-lua.cc
    src/zippylog/protocol/request.pb.cc
    src/zippylog/protocol/request.pb-lua.cc
    src/zippylog/protocol/response.pb.cc
    # TODO fix bug in lua-protocol where headers from imported files aren't included
    #src/zippylog/protocol/response.pb-lua.cc
    src/zippylog/request_processor.cpp
    src/zippylog/request_processor.pb.cc
    src/zippylog/request_processor.pb-lua.cc
    src/zippylog/store.cpp
    src/zippylog/store_watcher.cpp
    src/zippylog/stream.cpp
    src/zippylog/util.cpp
    src/zippylog/zeromq.cpp
    src/zippylog/zippylog.cpp
    src/lua-protobuf.cc
)
set(libzippylog_link_libraries
    ${LUA_LIBRARIES}
    ${PROTOBUF_LIBRARY}
    ${ZEROMQ_LIBRARY}
)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(libzippylog_link_libraries ${libzippylog_link_libraries} rt)
endif()

# produce libzippylog shared library
add_library(libzippylog SHARED ${libzippylog_sources})
target_link_libraries(libzippylog ${libzippylog_link_libraries})
set_target_properties(libzippylog PROPERTIES OUTPUT_NAME zippylog)

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    if (CMAKE_SYSTEM_VERSION VERSION_LESS 11.0.0)
        message(FATAL_ERROR "zippylog requires OS X 10.7 (Lion) or higher")
    endif()

    set_target_properties(libzippylog PROPERTIES LINK_FLAGS "-framework CoreServices")
endif()

#####################
# libzippylog Tests #
#####################

set(libzippylog-test_sources
    src/zippylog/testing.cpp
    src/zippylog/libzippylog-test.cc
    src/zippylog/client_unittest.cpp
    src/zippylog/device/server_unittest.cpp
    src/zippylog/device/persisted_state_manager_unittest.cpp
    src/zippylog/device/string_loader_unittest.cpp
    src/zippylog/envelope_unittest.cpp
    src/zippylog/lua_unittest.cpp
    src/zippylog/platform_unittest.cpp
    src/zippylog/request_processor_unittest.cpp
    src/zippylog/store_unittest.cpp
    src/zippylog/stream_unittest.cpp
)

add_executable(libzippylog-test ${libzippylog-test_sources})
target_link_libraries(libzippylog-test ${libzippylog_link_libraries} libzippylog ${GTEST_LIBRARIES})

#####################
# zippylogd program #
#####################

add_executable(zippylogd src/zippylogd.cpp)
target_link_libraries(zippylogd libzippylog)

#######################
# zippylogcat program #
#######################

add_executable(zippylogcat src/zippylogcat.cpp)
target_link_libraries(zippylogcat libzippylog)

##########################
# zippylogclient program #
##########################

add_executable(zippylogclient src/zippylogclient.cpp)
target_link_libraries(zippylogclient libzippylog)

##################################
# zippylog_string_loader program #
##################################

add_executable(zippylog_string_loader src/zippylog_string_loader.cpp)
target_link_libraries(zippylog_string_loader libzippylog)

#########################
# zippylogbench program #
#########################
add_executable(zippylogbench src/zippylogbench.cpp src/zippylog/testing.cpp)
target_link_libraries(zippylogbench libzippylog ${GTEST_LIBRARIES})

