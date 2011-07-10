This document describes the zippylog protocol as it relates to developers. The actual protocol definition is defined by the .proto source files located in proto/ directory and the documentation on the zippylog wiki.

# Design Guidelines

## Versioning

The zippylog protocol is not strictly globally versioned (at least not yet). Instead, the 0MQ message format is versioned and individual request and response messages have a verion identifier in the message name.

The protocol buffer message definitions do not change once they are shipped in a stable release. The rationale here is backwards compatibility. If a message or the semantics need changed, a new message type should be created.

## Naming

Request message names should be actions/verbs. Response messages should be things/nouns or state. For both types of messages, a version identifier is typically added at the end of the message. e.g. *V1*.

## Limited Functionality

Don't try to do too much with a single message, especially request messages. Operations should be small and well-defined. Complex operations are harder to implement and more prone to bugs and confusion.

e.g. go with WriteEnvelope() and WriteEnvelopeFromMessages() instead of a single message that allows either an envelope or a set of messages.
