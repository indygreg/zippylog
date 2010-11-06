# zippylog - A Fast and Efficient Message Bus and Store

This document is targeted to end users wishing to install zippylog on a system.

For details about the zippylog project, including why you may want to use it,
visit the [project wiki](http://wiki.github.com/indygreg/zippylog/).

## System Requirements

You'll need to [install](http://code.google.com/apis/protocolbuffers/)
Google Protocol Buffers, version 2.3.0 or later. Be sure to install the Python
bits in addition to the C++ tools and libraries.

In theory, on Debian and Ubuntu, you could run:

 > apt-get install protobuf-compiler python-protobuf

But as of this typing, the repositories only had version 2.2.x, which is not
compatible at this time.

You'll also need Python 2.6 or later, as zippylog relies on a few features
introduced in 2.6.

## Installing

1. Grab the source code. Either clone the git repository or find the
_Download Source_ link on github to grab an archive. Hopefully that's a
one-time process.

2. Install Python tools and modules.

`python setup.py install`

You should now have a `zippylog_compile` script available on your system.

## Defining Messages

Next, you'll need to create your domain-specific Protocol Buffer message
definitions for the messages you wish to record.

Start by creating a directory to hold your definitions.

`mkdir ~/zippylog-messages`

`cd ~/zippylog-messages`

You'll create .proto files in this directory.

The directory structure in this directory determines the namespace of
messages. The first directory level is the message namespace. This likely
corresponds with your company or organization name. i.e. if you are the
_ACME_ company, you'll probably want to create an _acme_ directory at
the root.

Under your namespace directory, create additional directory layers as are
necessary.

At any directory, create .proto files containing your message definitions.
The name of the file will form a final namespace member.

For small organizations with limited amounts of message types, you may want
a flat namespace:

* /acme/webapp.proto
* /acme/db.proto

Or, if there are many message types per application, you may wish to group them
additionally:

* /acme/webapp/errors.proto
* /acme/webapp/billing.proto
* /acme/webapp/performance.proto

For larger organizations, you may wish to add some corporate hierarchy:

* /acme/operations/router_stats.proto
* /acme/operations/system_stats.proto
* /acme/sales/purchase_order.proto

The protocol buffer
[Language Guide](http://code.google.com/apis/protocolbuffers/docs/proto.html)
is the definitive source for creating .proto files.

Once you have defined your messages, you'll need to run _zippylog_compile_ to
generate bindings to zippylog.

> zippylog_compile ~/zippylog-messages ~/zippylog-generated

Like the message source directory, you'll likely want to have the output
directory under version control. That way, in case you do something silly,
you can easily revert changes.

