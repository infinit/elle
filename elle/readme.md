Elle, the Infinit utilities.

# About:

Elle is a library containing easily reusiable elements, e.g.:

- [Buffer](https://github.com/infinit/elle/elle#Buffers)
- [IOStream](https://github.com/infinit/elle/elle#IOStream)
- [log](https://github.com/infinit/elle/elle#log)
- [serialization](https://github.com/infinit/elle/elle#serialization)

## Buffers

Buffers represent memory. There are 3 types of buffers:
- elle::Buffer (that owns the memory)
- elle::ConstWeakBuffer (A read-only, glorifiend C array pointer)
- elle::WeakBuffer (Like ConstWeakBuffer, but mutable)

Each buffer has a stream API (via [StreamBuffer](https://github.com/infinit/elle/elle#StreamBuffer)), can be iterated, supports indexing, comparison and hashing, direct memory access and can be converted to std::string.

```
// Construct a Buffer.
elle::Buffer buffer("something interesting");
std::cout << buffer[3] << std::endl; // e

// Construct a ConstWeakBuffer on a slice of the original buffer memory.
elle::ConstWeakBuffer slice(buffer.contents() + 4, 2);
std::cout << slice.string() << std::endl; // th

// Construct a WeakBuffer on another slice of the original buffer memory.
elle::ConstWeakBuffer mslice(buffer.contents() + 6, 2);
std::cout << mslice.string() << std::endl; // in

// Edit the memory.
mslice[1] = 'o';

// The original buffer benefits from the change done previously.
std::cout << buffer[3] << std::endl; // o
std::cout << buffer.string() << std::endl; //xxx
```
[code](https://github.com/infinit/elle/elle/examples/buffer.cc)

## Serialization

Elle includes a serialization library designed to support modern C++, including smart pointers, inheritance and provide versionning, the less intrusive way possible. It supports naively most of the basic C++ objects, most elle objects and is easly extendable via a conversion API.

```

## Traits

// Printable.

## Logs

```
ELLE_LOG_COMPONENT("component1");

// [...]

// Report if the log level is set to at least LOG (default).
ELLE_LOG("Something super relevant with two parameters (%s) and this one (%s)", 42, elle::Buffer("buffer"));
// Report if the log level is set to at least TRACE.
ELLE_TRACE("Something relevant");
// Report if the log level is set to at least DEBUG.
ELLE_DEBUG("Something less relevant");
// Report if the log level is set to at least DUMP.
ELLE_DUMP("Something only relevant for nerdz");

ELLE_DEBUG("The log in the following scope will have indented")
{
  ELLE_DEBUG("< whites spaces");
}

{
  // Nested log component, replacing "component1".
  ELLE_LOG_COMPONENT("component2");
  ELLE_ERR("Something extremely wrong");
  ELLE_WARN("Something wrong");
}
```
[code](https://github.com/infinit/elle/elle/examples/log.cc)

The program will output different in information according to the value of ELLE_LOG_LEVEL.
```
./log # default: ELLE_LOG_LEVEL=LOG
[component1] [main] Something super relevant with two parameters (42) and this one (buffer)
[component2] [main] Something went terribly wrong.
[component2] [main] Something went wrong.

ELLE_LOG_LEVEL=component1:DUMP,component2:NONE ./log
[component1] [main] Something super relevant with two parameters (42) and this one (buffer)
[component1] [main] Something relevant
[component1] [main] Something less relevant
[component1] [main] Something only relevant for nerdz
[component1] [main] The log in the following scope will have indented
[component1] [main]   < white spaces
```
