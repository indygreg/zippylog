zippylogd is a daemon that can be used for writing zippylogs and/or serving client requests.

It has the following modes of operation (any combination can be active in any instance of zippylogd):

* server - will bind to at least one 0MQ socket and process client requests
* piped log writer - will receive log entries on a pipe (typically stdin) and will write them out
* socket log writer - will receive log entries on a 0MQ socket and will write them out

# Server Mode

Server mode is enabled by adding the --server flag to the command arguments. The argument after this flag is required and must be a path to a config file.

The config file is a Lua script which will be executed by the server. Specifically-named global variables will configure the server.

## Variables

### Basic Settings

* store_path - Filesystem path of stream store. Currently, this must be defined.
* endpoints - 0MQ endpoints to which to listen for requests on. Defined as a table of strings (keys are not relevant). Values are fed into 0MQ's bind(), so they should look like *tcp://127.0.0.1:52484*, etc.

### Performance Settings

* worker_threads - Integer number of threads that will process client requests. Every request message received will be processed by a worker thread. Requests are processed in a FIFO manner. Each thread processes one request at a time. The maximum number of parallel requests that can be processed is thus defined by this value.
* streaming_threads - Integer number of threads that process streaming events to clients. Streaming requests are associated with a single streaming thread at subscription acceptance time. All streaming subscriptions are load balanced across the available streaming threads. Each streaming thread can handle multiple simultaneous subscriptions.
* streaming_subscription_ttl - Integer milliseconds for how long streaming subscriptions remain alive without a keepalive. Clients are required to _renew_ subscriptions or they expire. This defines the number of milliseconds after which an unrenewed subscription will expire. Defaults to 60000, or 1 minute.
* stream_flush_interval - How frequently to flush written streams, in milliseconds. Defaults to 5,000, or 5 seconds. Streams may flush more often than this, depending on activity. This just defines the max interval at which flushing should occur.

### Lua Settings

* lua_execute_client_code - Boolean that determines whether the server can execute client-supplied Lua code. False by default, because most secure by default is a good practice.
* lua_streaming_max_memory - Integer size in bytes that individual interpreters associated with streaming are allowed to grow to.

### Logging Settings

* log_bucket - String for bucket where zippylogd writes its logs
* log_stream_set - String for stream set where zippylogd writes its logs
    
# Pipe Logger

Piped logger mode is enabled by adding --piped to the command arguments. In piped logger mode, zippylog reads messages to log from a pipe, typically stdin.

In piped mode, the following arguments have meaning (optional unless specified otherwise):

* --store <path> - Specifies path to store that we should log to (required)
* --path <store path> - Specifies a store path to a stream set to which we should write. e.g. "/bucket/set"
* --piped-lua-file <path> - Path to a Lua file which will be loaded into the process to enhance functionality
* --piped-lua-max-memory <size> - Size (in kilobytes) that piped Lua interpreter can consume. Default is 1024, or 1 megabyte. This should be sufficient for normal uses.

## Lua Integration

If Lua code is loaded into the server in piped mode, Lua functions can influence logging. Functions that can be defined are:

* zippylog_process_line

For details of how each function works, see lua-api.
