# pblog - Structured Event Logging Using Protocol Buffers

pblog aims to build a logging system utilizing Google's Protocol Buffers.

The goals are to create a logging system that is:

* fast
* robust (in terms of reliability)
* easy-to-use

## Overview

For each event type you wish to log, you define a Protocol Buffer message
type that describes it. For example, a _user login_ action might have an
_username_ and _last login_ field. You register these custom events with
pblog. At application run time, you instantiate instances of your messages,
populate the fields, and pass them along to pblog. pblog optionally adds some
useful metadata that is common for logging systems and writes out the message
_somewhere_.

pblog contains a server process that allows clients to interact with
written log data. Clients can pull log data, push data between servers, etc.
This provides the necessary infrastructure to create a log aggregation and
distribution network.

## Why Use pblog?

1. It is structured. This makes it extremely simple for machines to consume.
This means you can easily pipe it into your data warehouse, monitoring
infrastructure, etc.

2. It is fast. It is because Protocol Buffers are
[fast](http://wiki.github.com/eishay/jvm-serializers/).

I can't overstate the structured bit enough. How many times have you toiled
trying to import yet another data source into some system? With pblogs, the
data format is the same across all data, so it is only a matter of implementing
the business logic to decide how to treat pieces of data, which you have to do
anyway.

## Supported Programming Languages

pblog is a light layer on top of Protocol Buffers. Therefore, it should be
possible to write pblog logs from any programming language that
[supports](http://code.google.com/p/protobuf/wiki/ThirdPartyAddOns) Protocl
Buffers.

To produce pblogs, you'll need a pblog writer.

## Technical Info

### Common Message Container

Each custom message you write is wrapped in a common message container. This
container, which is strongly defined by the pblog project, contains numerous
optional fields which are useful for logging. All fields are optional, and
because of the Protocol Buffer wire format, there is no penalty for defining
but not using them.

Common fields include:

* create time - the time the event occurred, in microseconds since UNIX epoch.
You almost certainly want to use this, for hopefully obvious reasons.

* primary and secondary keys - you can associate one primary and multiple
secondary keys (arbitrary byte sequences) with each event. These keys can be
used to correlate events.

* level - numeric log level. Many existing loggers (especially in the log4*
family) associate a numeric level (often mapped to a string like _INFO_,
_WARN_, etc) to each event. pblog allows you to do the same. We don't want
to enforce any numeric convention on you, so you just get an unsigned 32 bit
integer to do whatever you want.

* writer metadata - This allows log writers to put their _stamp_ on the event.
This _stamp_ may include the time the event was written, the hostname the
event was written on, the application that caused the event to be written, etc.

### Requirements

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

Currently, pblog requires that **each .proto file have a _package_
definition**. The package name should resemble the the directory structure.

e.g. _/acme/WebApplication/basic.proto_ might look like

> package acme.WebApplication.basic;
>
> message SampleEvent {
> ...

This arbitrary restriction may be lifted in the future.

Once you have defined your messages, you'll need to generate some more .proto
files, which are used by pblog. You do this by running the _pblog_compile_
program, which is distributed as part of pblog.

> pblog_compile --help
> Usage: pblog_compile [options] /path/to/messages /output/path
>
> Generates Protocol Buffer extensions for all defined messages

>Options:
>  -h, --help            show this help message and exit
>  -p PROTOC, --protoc=PROTOC
>                        Path to protoc binary

> pblog_compile ~/pblog-messages ~/pblog-generated

Like the message source directory, you'll likely want to have the output
directory under version control. That way, in case you do something silly,
you can easily revert changes.

## Shortcomings

Since event message types are effectively enumerated types, parsing said
fields in the general event message container requires total agreement
between the producer and consumer. Inside a corporate environment where the
individual event message types are likely centrally maintained, this is not
a problem. But, if we move out to the internet, exchange of event information
across multiple users of pblog will likely be problematic, as two companies
_X_ and _Y_ will likely have two different message types having numeric id
_N_.

Logging data is typically site-specific and it would be trivial to write
a _proxy_ that translated event ids on the fly, so I'm not too concerned
about this shortcoming at the moment. That being said, I can see the desire
to exchange log events between sites with little effort, so supporting
this is definitely a desirable feature.

## Background

Logs are typically produced as unstructured text with metadata inside. For
example, let's take the output of a typical log4*-style logger, such as
Python's logger package:

 > 2010-07-24 13:37:12,323 - acme - INFO - response to 'lucy' served in 0.24 seconds

The real meat of this event is the data:

 * time event produced (_2010-07-24 13:37:12,323_)
 * what produced it (_acme_)
 * importance level (_INFO_)
 * what (_response served_)
 * to who (_lucy_)
 * how long (_0.24 seconds_)

That took 82 bytes to encode. And, it is relatively difficult for a computer
to parse. In fact, a human needs to instruct a computer how to parse it, likely
by using regular expressions (which are relatively slow), or tools such as
_cat_, _grep_, _awk_, _sed_, etc. This works for a while, but it doesn't scale
to large datasets. You'll quickly find things bogged down by network
bandwidth limitations, CPU overhead of decoding messages, pain of managing
various parsing scripts, etc.

### Why Protocol Buffers?

When I set out to create a structured logging system, there were a number of
technologies I could have used. The most compelling was
[Apache Thrift](http://incubator.apache.org/thrift/) and
[Facebook Scribe](http://github.com/facebook/scribe). Thrift can be thought of
as Facebook's/ASF's version of Protocol Buffers. And, Scribe is a nifty little
server that aggregates logs. Perfect, right? Well, no.

Unfortunately, Scribe subscribes to the _unstructed text logging_ model, as its
[Log() interface](http://wiki.github.com/facebook/scribe/logging-messages)
takes a string category and string message. In other words, you write a
_char *_ to a named bucket. Yes, _string_ types in Thrift are simply byte
sequences of a specific length, so you could have the _message_ parameter be
a binary-encoded Thrift struct. I did try this. But it felt really hacky.
Because of the tightly-coupled architecture of Thrift's protocol and transport
layers, I had to create a custom transport that held the encoded data in a
buffer and double Thrift-encode data. It just didn't feel right.

In addition, I also don't like the Scribe model for writing logs. Your
application needs to establish a socket to the Scribe daemon to write logs.
Yes, it is probably running on localhost and is thus probably pretty quick.
But, I found myself having to write tons of error handling code to handle
socket connectivity issues, which frequently presented. From a reliability
and service dependency perspective, I didn't like it. I'll just write logs
to disk or similar so-simple-it-won't-fail backend, thank you.

Finally, I found the documentation and ease-of-use of Protocol Buffers to
be superior to Thrift's. 

Also, I could have just written out JSON, YAML, XML, etc, but since Protocol
Buffers are so fast and small, I couldn't live with myself if I had sacrificed
those qualities.
