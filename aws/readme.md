# AWS: reactorified Amazon Web Service API

The AWS library provides a *reactorification* of the [Amazon Web Services](https://aws.amazon.com) APIs, mostly Amazon [Simple Storage Service](https://aws.amazon.com/s3) a.k.a Amazon S3.

The reactor is part of the [Elle](https://github.com/infinit/elle) set of libraries.

## Motivation

The [reactor](/reactor) provides a reactorified HTTP(s) capabilities, the [cryptography](/cryptography) provides tools to generate signatures. With both libraries, we've created a reactorified library to performs AWS operations.

## How to compile

_See [Elle: How to compile](https://github.com/infinit/elle#how-to-compile)._

```bash
./drake //aws/build -j 2
```
