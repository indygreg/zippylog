This document describes general considerations for developing against zippylog.

# Repository Layout

The Git repository has the following layout:

* / - Location for files related to building, licensing, support, etc.
* /src - Where all the C++ source code can be found
* /lib - Contains some Python code. This hasn't been maintained in a while.
* /proto - Contains protocol buffer definitions used by zippylog
* /bin - Support scripts and executables
* /doc - Documentation for this release of zippylog
* /test - Supports the automated testing tools
* /msvc - Visual Studio solution and project files
* /external - Location for Git submodules and external dependencies

# C++ Source Files

The /src/ tree contains a number of C++ source and header files.

The set of source files can be divided into the following classifications:

* Core classes and utilities
* Devices
* Generated protocol buffer message definitions
* Unit tests
* Executables

These can be thought of as a layered onion. At the core are the core classes
and generated protocol buffer messages. Built on top of that are devices,
which take the core classes and build something that performs a specific role.
On top of that are executables, which provide a means to launch devices
without any extra coding.

# Message Enumeration Class Members

The generated protocol buffer message classes have static const uint members
which are initialized to the enumerated value. e.g.

    class Foo {
        ...
        static const uint32 zippylog_enumeration = 5;
    };

This is compiler shorthand for declaring the member in the header and defining in the source file:

    const uint32 zippylog_enumeration = 5;

This works fine on most compilers, including GCC and the Microsoft compiler.

However, we encounter an issue if we attempt to use a reference of the member.

    &MyMessage::zippylog_enumeration;

Compilers, at linking time, will complain about an undefined reference.

The question at http://stackoverflow.com/questions/3025997/c-defining-static-const-integer-members-in-class-definition
sums up the problem.

Currently, the workaround is to not use references of the members. Simply
create a new variable off the stack and assign the enumerated value to it.

In the future, we may provide a better solution to this problem which doesn't
require a workaround.
