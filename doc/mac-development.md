OS X is a fully supported development platform for zippylog.

At this time OS X 10.7 (Lion) is required to build zippylog. Support for
previous versions should be possible with minimal work in the platform code.
The specific component requiring 10.7 is new constants introduced in
FSEvents.h, which is used by the directory watcher.

# Required Programs

To build zippylog, you'll need the following:

* Xcode (specifically the OS X SDK). Obtain Xcode through the App Store.
* CMake. An easy way to obtain CMake is to install Homebrew then
*brew install cmake*

You will additionally need the following libraries:

* Lua
* Protocol Buffers
* ZeroMQ

The recommended versions of each will be defined as Git submodules or will
be documented in general-development.md.

# Configuring Your Build Environment

The easiest way to get started is to run CMake and address problems as they
are reported.

## Install Lua

The Lua libraries can be installed via Homebrew:

  brew install lua

Eventually, zippylog will automagically support LuaJIT if it is available, so
you might want to install it while you are at it:

  brew install luajit

This should satisfy the Lua dependency.

## Install ZeroMQ

ZeroMQ has many different versions and the package provided by Homebrew may
not be supported by zippylog. Therefore, it is recommended to compile ZeroMQ
manually. If you compile from the sources in the Git submodule, zippylog's
CMake config will find and use it automagically.

To build the version of ZeroMQ that zippylog is optimized for:

  git submodule update --init
  cd external/zeromq
  ./configure
  make
  cd ../..

## Install Protocol Buffers

zippylog is currently optimized for Protocol Buffers 2.4.1. If your package
distrubution source has this version, it is probably safe to install. If using
Homebrew, just:

  brew install protobuf

Or, if you want to compile and install it yourself, first grab the sources:

  svn co -r 391 http://protobuf.googlecode.com/svn/tags/2.4.1 external/protobuf

(This obviously requires Subversion.)

Then compile and install the C++ libraries and tools:

  cd external/protobuf
  ./autogen.sh
  ./configure
  make
  make install

Then build and install the Python bindings:

  cd python
  python setup.py install

## Install Google Test

zippylog uses the Google Test framework for tests. If you have Homebrew, you
can probably just install GTest from there:

  brew install gtest

If not, you can compile the sources from the Git submodule:

  git submodule update --init
  cd external/googletest
  cmake .
  make
  make install

## Building zippylog

Once you have all the dependencies, you should simply be able to run CMake
followed by make:

  cmake .
  make

This will build the following targets:

* libzippylog - Core C++ library
* libzippylog-test - Unit test executable
* zippylogbench - Benchmark executable
* zippylogd - zippylogd server/daemon
* zippylog_string_loader - tool to load strings into zippylog
* zippylogcat - "cat" for zippylog
* zippylogclient - Executable that acts as a client to servers.

If zippylog builds properly, the first thing you should do is run the test
suite to verify everything is working:

  ./libzippylog-test

If that works, congratulations, you have a working setup!

## Regarding Installation

zippylog currently does not define an *install* target for make. This will
come later, once the project has matured.

