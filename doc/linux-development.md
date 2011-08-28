This document describes how to get started with zippylog development on Linux.

# General Steps

1. Clone zippylog Git repository
2. Install required dependencies
3. Build

# Installing Dependencies

zippylog on Linux requires the following dependencies:

* Protocol Buffers 2.3.0 or later
* Lua 5.1 (or any Lua interpreter featuring the official C API)
* ZeroMQ 2.0.10 (or later)
* pthreads
* Google Test (if you want to run unit tests)
* CMake (to build)
* GCC

zippylog is currently primarily developed and tested on 64 bit editions of Ubuntu 10.04 and 10.10. It will likely work on other distros, it just hasn't been fully tested yet.

Most of zippylog's library dependencies are included as Git submodules. If you obtain source this way, you should get the most appropriate (i.e. tested) version of the package.

    git submodule update --init

Currently, only protocol buffers is not included as a submodule. However, you can easily pull it from Subversion:

    svn co -r 391 http://protobuf.googlecode.com/svn/tags/2.4.1 external/protobuf

## Protocol Buffers

If you are running Ubuntu 10.10 with a standard APT sources list, you can install the required protocol buffer components as follows:

    apt-get install libprotobuf6 libprotobuf-dev python-protobuf protobuf-compiler

For others, you'll need to obtain the protocol buffer source, compile, and install it. See http://code.google.com/p/protobuf/

## Lua

Lua 5.1 has been around for a while and chances are your distribution includes it in its packaging system.

On Ubuntu:

    apt-get install liblua5.1-0 liblua5.1-0-dev

If you can't find a package, head over to http://www.lua.org/ to download the sources.

## ZeroMQ

At the time of this writing, ZeroMQ was still pretty new and support in distributions was either lacking or behind ZeroMQ's rapid release cycle. You can try your luck with your distro's packages (we need at least 2.0.10), or you can compile it pretty easily.

On Ubuntu 10.10, the packages to install are:

    apt-get install libzmq-dev libzmq0

At the time of this writing, they were older than 2.0.10, so check the version before installing.

See http://www.zeromq.org/ for additional instructions on how to build ZeroMQ. It is worth noting that zippylog does not use multicast, so it doesn't require the PGM extension to ZeroMQ.

## CMake

CMake is an alternate build system. It has been around for a little while, so your distro should have a package for it.

On Ubuntu:

    apt-get install cmake

## Google Test

Google Test is zippylog's C++ unit testing framework. Some distros have a package for it.

On Ubuntu:

    apt-get install libgtest0 libgtest-dev

## Misc

libuuid is not available on all systems by default. On Ubuntu:

    apt-get install uuid-dev
    
Doxygen is used to build the documentation. On Ubuntu:

    apt-get install doxygen

# Configuring and Compiling

Configuring and compiling is made possible through CMake. Simply run:

  cmake .

If this succeeds, on most platforms this will produce a Makefile for the project. Simply run *make* to compile zippylog.

