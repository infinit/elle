# AWS: reactorified Dropbox API

The Dropbox library provides a reactorification for the [Dropbox](https://www.dropbox.com/developers) API.

This library is part of the [elle](https://github.com/infinit/elle) set of libraries.

## Motivation

To demonstrate capabilities our [Filesystem](https://github.com/infinit/fs) we've created a wrapper for Dropbox. This libraries is just a backend, wrapping the Dropbox web API using the [reactor](/reactor).

## How to compile

_See [elle: How to compile](https://github.com/infinit/elle#how-to-compile)._

```bash
./drake //dropbox/build -j 2
```
