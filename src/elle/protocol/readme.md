# Protocol

Protocol is a library dedicated to construct communication channels on top of streams. It unifies protocol structure and provides a support for [Remote Procedure Call](https://en.wikipedia.org/wiki/Remote_procedure_call).

> Protocol is part of the [Elle](https://github.com/infinit/elle) set of libraries.

## Motivation

The [reactor](/src/elle/reactor) allows for establishing connection via sockets. From there, we need to set up a robust and sustainable solution to communicate. Protocol provides unification and versioning for communications.

The `ChanneledStream` acts as a [multiplexer/demultiplexer](https://en.wikipedia.org/wiki/Multiplexing). You can have an unlimited number of separated and manageable channels in a single stream.

> *Notes:*
> - The current RPC system is deprecated in a favor a new one that will be integrated soon.

## How to compile

_See [Elle: How to compile](https://github.com/infinit/elle#how-to-compile)._

```bash
./drake //src/elle/protocol/build -j 2
```

## Maintainers

 * Website: https://infinit.sh/open-source
 * Email: open+elle@infinit.sh
