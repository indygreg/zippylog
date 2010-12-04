This document describes the zippylog protocol as it relates to developers. The actual protocol definition is defined by the .proto source files and the documentation on the zippylog wiki.

# Design Guidelines

## Versioning

The zippylog protocol is not strictly globally versioned (at least not yet). Instead, the 0MQ message format is versioned and individual messages can opt to have a verion identifier.

Protocol buffer messages constituting the protocol should be as backwards compatibile as possible and should leave the door open for future refactoring. This means a number of things:

* declare fields as optional, never required
* consider adding a _version_ field to the message that allows clients to tell which version features/semantics to allow/follow.

If message cannot be versioned cleanly, create a new message type instead of hacking things into the existing message.

## Naming

Request message names should be actions/verbs. Response messages should be things/nouns or state.

## Limited Functionality

Don't try to do too much with a single message, especially request messages. Operations should be small and well-defined. Complex operations are harder to implement and more prone to bugs and confusion.

e.g. go with WriteEnvelope() and WriteEnvelopeFromMessages() instead of a single message that allows either an envelope or a set of messages.
