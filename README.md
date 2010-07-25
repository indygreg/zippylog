# pblog - Structured Event Logging Using Protocol Buffers

pblog aims to build a logging system that uses Google's Protocol Buffers for
encoding.

## Overview

pblog defines a Protocol Buffer message type which represents individual log
events. The type instance contains common fields that are frequently associated
with log events and a site-specific message. This site-specific message is
itself a Protocol Buffer message type, which you define in your local
deployment. In other words, for each event you wish to log (e.g. _user login_,
_update profile_, etc), you define a new Protocol Buffer message that contains
metadata for that specific message. Inside your application, you create a new
instance of that message type and send it to an instantiated pblog logger.

### Event Messages

Each log event message may contain the following common fields:

 * create_time - time event was produced
 * primary_key - arbitrary key used to correlate events
 * secondary_keys - list of additional keys used to correlate events
 * writers - metadata describing writers that touched event
 * level - numeric logging level

In addition, each message contains a site-defined Protocol Buffer message,
which can be anything you want.

### Requirements

You'll need to [install](http://code.google.com/apis/protocolbuffers/)
Google Protocol Buffers, version 2.3.0 or later. Be sure to install the Python
bits in addition to the C++ tools and libraries.

In theory, on Debian and Ubuntu, you could run:

 > apt-get install protobuf-compiler python-protobuf

But as of this typing, the repositories only had version 2.2.x, which is not
compatible at this time.

You'll also need Python 2.6 or later, as that is what the current tools are
written in.

## Deploying and Running

TODO (when things stabilize)

## Shortcomings

Since event message types are effectively enumerated types, parsing said
fields in the general event message container requires total agreement
between the producer and consumer. Inside a corporate environment where the
individual event message types are likely centrally maintained, this is not
a problem. But, if we move out to the internet, exchange of event information
across multiple users of pblog will likely be problematic, as two companies
_X_ and _Y_ will likely have two different message types having numeric id
_N_.

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
to parse it. In fact, a human needs to instruct a computer how to parse, likely
by using regular expressions (which are relatively slow), or tools such as
_cat_, _grep_, _awk_, _sed_, etc. This works for a while, but it doesn't scale
to large datasets. You'll quickly find things bogged down by network
bandwidth limitations, CPU overhead of decoding messages, pain of managing
various parsing scripts, etc.

### Why Protocol Buffers?

When I set out to create a better logging system, there were a number of
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
be superior to Thrift's. Extensions, in particular, make my life much
more pleasant.
