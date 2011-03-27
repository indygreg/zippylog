This document details how Lua code can operate within zippylog.

# Parsing Lua Code

When zippylog receives Lua code, it first loads it into a Lua interpreter. The code is compiled by the interpreter. It is highly likely that if the Lua code could not be compiled, the operation will abort immediately.

After the Lua code is loaded, zippylog looks for specifically named variables in the global environment. These variables are Lua functions which represent callbacks that are executed at various phases or variables that tell zippylog how to execute.

The heuristics for determining the abilities defined in your Lua code are as follows:

1. Attempt to read the variable *zippylog_options* from the global environment. If this variable is defined and is a table, zippylog uses it to determine the code's abilities. See the section below for the format of this table. If this variable is a function, then it should return a table that defines the code's abilities.
2. Test for existence of specifically named functions in the global environment. Abilities are detected by the presence of the functions it uses.

# Available Libraries

Lua interpreters inside zippylog either have no libraries or very few libraries loaded by default. This is a security feature. Since the process can run user-supplied code, we'd rather start with a secure system than an insecure one.

Libraries can be enabled through various mechanisms. See the process documentation for the specifics.

# Envelope API

You'll likely encounter a zippylog envelope inside Lua. Envelopes are typically passed as function parameters or returned as a result of an operation.

Envelopes can be created in the following manner:

    -- construct a new, empty envelope
    -- equivalent to the default constructor in C++
    e = zippylog.envelope.new()

Envelope instances in Lua have the following API:

    -- assuming our envelope instance is stored in the local variable, e

    -- obtain the serialized value of the envelope (as string)
    serialized = e:serialize()

    -- obtain the number of messages embedded in the envelope
    count = e:message_count()

    -- obtain the namespace and type of the message at offset i, where i starts at 1
    namespace, type = e:message_enumeration(i)

    -- obtain the number of tags on the envelope
    count = e:tag_count()

    -- add a string tag to the envelope
    e:add_tag("foo")

    -- obtain the tag at offset i, where i starts at 1
    tag = e:get_tag(i)

    -- obtain the size of the serialized envelope
    size = e:serialized_byte_size()

    -- obtain the value in the string_value field or nil if not set
    s = e:get_string_value()

    -- set the string value to the specified string, s
    e:set_string_value(s)

    -- obtain the message at offset i, where i starts at 1
    message = e:get_message(i)

    -- add a message instance (see below for API) to an envelope
    e:add_message(m)

# Message API

Messages (the things inside envelopes) can be interacted with inside Lua.

Messages are protocol buffer message types. Therefore, their behavior is mostly governed by the [lua-protobuf](https://github.com/indygreg/lua-protobuf) project.

# Callback Functions

In this section, we describe the various Lua functions that can be executed during processing.

The function name is the default value looked for when no custom function name is defined using the *zippylog_options* table.

## zippylog_load_string(string)

Called when string data is being loaded into zippylog. This could be done via
zippylog_string_loader or a similar process.

This function performs a number of roles:

* Determine whether to accept the data
* Determines where to route the data (if applicable)
* Makes modifications to the string (as necessary)
* Splits the string into multiple components
* Converts the string into a set of messages
* Converts the string into a set of envelopes

The function communicates what it did via its return type pattern. It must
return one of the following type patterns:

* nil - the function didn't do anything. Calling tool defines the appropriate behavior
* true - the string should be loaded as-is. If changes were made to the string, they won't be reflected.
* false - the string should not be loaded.
* string - use the returned string value as the new string.
* string, string, ... - any number of strings. Each string is independent and has no association with each other. It is as if the strings were loaded separately.
* protocol buffer message - message representation of this string
* message, message, ... - any number of protocol buffer messages to be added to one envelope
* envelope - a specific envelope to produce
* envelope, envelope, ... - any number of envelopes
* table, any of above - table controls routing based on the keys *bucket* and *set*

When string data is returned (includes true, false, and possibly nil cases),
the behavior is for a new envelope to be created that has the string data from
this function for each string emitted.

When a protocol buffer message is emitted, a new envelope is created containing
that message or list of messages.

When an envelope is emitted, that envelope is used verbatim.

Here are some examples (we assume the string standard library is availble):

    -- forward lines beginning w/ "apache" as-is and drop everything else
    function zippylog_load_string(s)
        if string.sub(s, 0, 6) == "apache" then
            return true
        end

        return false
    end

    -- reverse the input string
    function zippylog_process_line(s)
        return string.reverse(s)
    end

    -- route strings based on the first 5 characters
    function zippylog_load_string(s)
       return { ["set"] = string.sub(s, 0, 5) }, true
    end

## zippylog_filter_envelope(envelope)

This function is called when filtering envelopes. The callback's job is to
determine whether the envelope is appropriate to pass on.

This function can return one of the following type patterns:

* true - the envelope passes the filter
* false - the envelope does not pass the filter