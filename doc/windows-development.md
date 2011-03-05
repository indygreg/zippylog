Windows is fully supported as a development environment for zippylog. The *msvc/* directory at the root level of the Git repository contains a Visual Studio 2008 solution and projects. It should be possible to build zippylog and all dependencies nearly out of the gate.

# Getting Started

1. Clone the zippylog Git repository
2. Initialize submodules (this obtains sources for Lua, ZeroMQ, GTest, etc)
3. Put Protocool Buffers 2.3.0 source tree at *external/protobuf/*
4. Open the Visual Studio Solution
5. Start working

This might look something like the following:

    git clone git://github.com/indygreg/zippylog.git
    cd zippylog
    git submodule update --init
    svn co -r 298 http://protobuf.googlecode.com/svn/tags/2.3.0 external/protobuf
  
# Visual Studio Configurations

There are a number of configurations in Visual Studio from which you can work with.

* Debug - Standard debug build. Produces shared libraries with debug symbols, no optimizations.
* Release - Optimized release build. Should be smaller and faster than the debug build. This is what you want to run in production.
* Unittest - Modified debug build to support running the unit tests

Finally, you have the choice among the following startup projects:

* zippylogd - The zippylogd executable. You'll need to tweak the debugging settings to define its arguments.
* zippylog-test - Launches Google Test suite. Will only work in the *Unittest* configuration.

# Projects

The following projects exist in the solution:

* gtest - Google C++ testing framework (required by the unit tests)
* liblua - Lua programming lanaguage shared library. Enables Lua integration.
* libprotobuf - Google's Protocol Buffers library. Always compiled statically.
* libzippylog - zippylog core routines library. This is the heart of zippylog.
* libzmq - ZeroMQ shared library.
* zippylog-test - zippylog unit tests. Generates an executable that can test zippylog.
* zippylog_string_loader - builds the zippylog_string_loader executable
* zippylogbench - builds the zippylogbench executable
* zippylogcat - builds the zippylogcat executable
* zippylogclient - builds the zippylogclient executable
* zippylogd - zippylogd program. Generates zippylogd.exe. Contains main() for zippylogd.

# Compiler Output

Generated files from the Visual Studio solution will be placed in a subdirectory of where the solution is. By default, this will be */msvc/*.

In this directory, there will be directory trees first for the system platform, then by the configuration. All libraries and executables will be placed in a common directory, so the runtime linker should have no problem finding dependencies.

e.g. if you are building the Debug Win32 version of zippylogd, you would find the binary at:

    /msvc/Win32/Debug/zippylogd.exe

# Static Library Hackiness

The generated protocol buffer C++ classes do not define the appropriate *__declspec(dllexport)* keyword required for the Microsoft compiler to export those symbols from shared libraries. Therefore, if we wish to utilize a protocol buffer message class from another project, we need to either compile the protocol buffer .cc files into that project or link a static library that has them.

It is much easier to link a static library than to manually add extra source files to many projects. So, a special build configuration for libzippylog exists that produces a static library and only a static library which contains all these symbols.
