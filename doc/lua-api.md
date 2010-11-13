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

# Callback Functions

In this section, we describe the various Lua functions that can be executed during processing.

The function name is the default value looked for when no custom function name is defined using the *zippylog_options* table.

## zippylog_process_line(string)

Called when a line of text is received. This is executed in the context of *a client sent a log message to zippylog for logging*.

This function can return one of the following type patterns:

* nil - the function didn't do anything. Default behavior depending on the situation is invoked
* true - the string should be forwarded as-is. If changes were made to the string, they won't be reflected.
* false - the string should not be forwarded.
* string - new string value to be forwarded
* string, string, ... - any number of strings. each is treated as a separate output
* protocol buffer message - message to be wrapped in an envelope for passing on

An optional table can be returned before one of the above patterns. If a table is returned, the following keys have meaning:

* bucket - bucket to write the message to
* stream_set - stream set to write the message to

Here are some examples (we assume the string standard library is availble):

    -- forward lines beginning w/ "apache" as-is and drop everything else
    function zippylog_process_line(s)
        if string.sub(s, 0, 6) == "apache" then
            return true
        end
        
        return false
    end
    
    -- reverse the input string
    function zippylog_process_line(s)
        return string.reverse(s)
    end
