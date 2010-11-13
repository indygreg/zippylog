This document describes general considerations for developing against zippylog.

# Message Enumeration Class Members

The generated protocol buffer message classes have static const uint members which are initialized to the enumerated value. e.g.

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

The question at http://stackoverflow.com/questions/3025997/c-defining-static-const-integer-members-in-class-definition sums up the problem.

Currently, the workaround is to not use references of the members. Simply create a new variable off the stack and assign the enumerated value to it.

In the future, we may provide a better solution to this problem which doesn't require a workaround.