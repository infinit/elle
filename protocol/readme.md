# Protocol

Protocol is a library dedicated to construct communication channels on top of streams. It unifies protocol structure and provides a support for [Remote Procedure Call]().

The reactor is part of the [elle](https://github.com/infinit/elle) set of libraries.

## Motivation

The [reactor](/reactor) provides a robust way to establish connection through sockets. From there, we need to setup a sustainable solution to communicate. Unification and versioning of the communication protocol it is mandatory for robustness. Protocol provides all those features.

The ChanneledStream acts as a [multiplexer/demultiplexer](https://en.wikipedia.org/wiki/Multiplexing). You can have an unlimited number of manageable and incorruptible channels in a single stream.

> *Notes:*
> - The current RPC system is deprecated in a favor a new one that will be integrated soon.

## How to compile

_See [elle: How to compile](https://github.com/infinit/elle#how-to-compile)._

```bash
./drake //protocol/build -j 2
```

## Further reading
