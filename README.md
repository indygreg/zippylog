# pblog - Structured Event Logging Using Protocol Buffers

## Requirements

You'll need to [install](http://code.google.com/apis/protocolbuffers/)
Google Protocol Buffers, version 2.3.0 or later. Be sure to install the Python
bits in addition to the C++ tools and libraries.

In theory, on Debian and Ubuntu, you could run:

 > apt-get install protobuf-compiler python-protobuf

But as of this typing, the repositories only had version 2.2.x, which is not
compatible at this time.

You'll also need Python 2.6 or later, as pblog relies on a few features
introduced in 2.6 (although I'm hard-pressed to name them).

## Deploying and Running

Step 1 is to grab the code. Either clone the git repository or find the
_Download Source_ link github to grab an archive. Hopefully that's a
one-time process.

Next, you'll need to create Protocol Buffer message definitions for the events
you wish to log.

Start by creating a directory to hold your definitions.

`mkdir ~/pblog-messages`
`cd ~/pblog-messages`

The directory structure under this root directory determines the namespace of
sorts for message types. You'll probably want to have at least one additional
layer, to help with grouping.

Some common layouts include:

> /:application1/
> /:application2/

or

> /:org name/:application1
> /:org name/:application2

For example,

> /WebApplication/
> /DatabaseApplication/

or

> /acme/NetworkData/
> /acme/Monitoring/

The number of layers is unlimited.

In the created directories, you'll create _.proto_ files containing Protocol
Buffer definitions. The
[Language Guide](http://code.google.com/apis/protocolbuffers/docs/proto.html)
is a great source for this.

Once you have defined your messages, you'll need to generate some more .proto
files, which are used by pblog. You do this by running the _pblog_compile_
program, which is distributed as part of pblog.

> pblog_compile --help
> <grok output here>
> pblog_compile ~/pblog-messages ~/pblog-generated

Like the message source directory, you'll likely want to have the output
directory under version control. That way, in case you do something silly,
you can easily revert changes.

