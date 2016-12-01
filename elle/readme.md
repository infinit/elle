# elle, the Infinit utilities.

elle is a library containing fundamental functionalities from buffers to memory
management and serialization, etc.

elle is part of the [Elle](https://github.com/infinit/elle) set of libraries.

## Motivation

C++ standard library lacks some of fundamental concepts, that's why the
[Boost](http://www.boost.org) libraries exist and tends to fill this lack of
functionality. Based on both, elle introduces new concepts and functionnalities
to makes developers life easier.

elle includes many helpers such as:

* [attributes](#attributes): Macros that automatically declares and implements setters and getters
* [buffers](#buffers): Abstract memory management
* [serialization](#serialization): json & binary serializations supporting versioning
* [logs](#logs): adjustable logs
* ...

## Examples

Here are [examples](examples):

### Attributes

Attributes are designed to avoid writing recurrent setter/getter patterns, easily add or remove attributes.

```cpp
struct Object
{
  ELLE_ATTRIBUTE(std::string, name); // Add a private attribute std::string _name.
  ELLE_ATTRIBUTE_R(uint32_t, value); // Add a private attribute uint32_t _value and a getter uint32_t value() const.
  ELLE_ATTRIBUTE_RW(bool, done); // Add a private attribute bool _done, a getter bool done() const and a setter void done(bool);
};

Object a;
std::cout << a.value() << std::endl;
a.done(true);
std::cout << a.done() << std::endl;
```
Full example [here](https://github.com/infinit/elle/elle/examples/samples/attributes.cc).

### Buffers

Buffers represent memory. There are 3 types of buffers:
- elle::Buffer (that owns the memory)
- elle::ConstWeakBuffer (A read-only, glorifiend C array pointer)
- elle::WeakBuffer (Like ConstWeakBuffer, but mutable)

Each buffer has a stream API, can be iterated, supports indexing, comparison and hashing, direct memory access and can be converted to std::string.

```cpp
// Construct a Buffer.
elle::Buffer buffer("something interesting");
std::cout << buffer[3] << std::endl; // e

// Construct a ConstWeakBuffer on a slice of the original buffer memory.
elle::ConstWeakBuffer slice(buffer.contents() + 4, 2);
std::cout << slice.string() << std::endl; // th

// Construct a WeakBuffer on another slice of the original buffer memory.
elle::WeakBuffer mslice(buffer.contents() + 6, 2);
std::cout << mslice.string() << std::endl; // in

// Edit the memory.
mslice[0] = 'o';
std::cout << mslice.string() << std::endl; // in

// The original buffer benefits from the change done previously.
std::cout << buffer[6] << std::endl; // o
std::cout << buffer.string() << std::endl; // somethong interesting
```

Full example [here](https://github.com/infinit/elle/elle/examples/samples/buffer.cc).

### Serialization

elle includes a serialization library designed to support modern C++, including smart pointers, inheritance and provide versionning, the less intrusive way possible. It supports naively most of the basic C++ objects, most elle objects and is easly extendable via a conversion API.

The following example shows versionned class Record, whose evolved from at versions *0.2.0* and *0.3.0*.
```cpp
struct Record
{
  Record(std::string const& name)
    : _name(name)
    , _id(elle::UUID::random())
    , _date(boost::posix_time::second_clock::universal_time())
  {
  }

  Record(elle::serialization::SerializerIn& in)
  {
    this->serialize(in);
  }

  void
  serialize(elle::serialization::Serializer& s,
            elle::Version const& version)
  {
    s.serialize("name", this->_name);
    s.serialize("id", this->_id);
    if (version < elle::Version(0, 2, 0))
    {
      this->_date = boost::posix_time::time_from_string("2016-10-10 00:00:00.000");
    }
    else
    {
      s.serialize("date", this->_date);
    }
    if (version < elle::Version(0, 3, 0))
    {
      s.serialize("tags", this->_tags);
    }
  }

private:
  std::string _name;
  elle::UUID _id;
  boost::posix_time::ptime date;
  std::vector<std::string> tags;
};
```
Full example [here](https://github.com/infinit/elle/elle/examples/samples/serialization.cc).

### Traits

#### Printable

```cpp
struct HitCounter
  : public elle::Printable
{
  HitCounter()
    : _hit(0)
  {
  }

  void
  print(std::ostream& out) const override
  {
    out << "This counter that has "
    if (this->_hit == 0)
      out << "never been hit";
    else if (this->_hit > 100)
      out << "been hit tones of times";
    else
      elle::fprintf(out, "been hit %s times", this->_hit);
  }
  ELLE_ATTRIBUTE_RW(int, hit);
};

HitCounter counter;
ELLE_ASSERT_EQ(elle::sprintf("%s", counter), "This counter that has never been hit");
counter.hit(42);
ELLE_ASSERT_EQ(elle::sprintf("%s", counter), "This counter that has hit 42 times");
counter.hit(1002);
ELLE_ASSERT_EQ(elle::sprintf("%s", counter), "This counter that has hit tones of times");
```
Full example [here](https://github.com/infinit/elle/elle/examples/samples/printable.cc).

### Logs

elle contains a logging library designed around *components* and *levels* designed to have the lowest overhead possible if components aren't enabled.

The example below demonstrates its basic concepts:

```cpp
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
Full example [here](https://github.com/infinit/elle/elle/examples/samples/log.cc).

The program will output differently according to the value of ELLE_LOG_LEVEL in the environment.

```bash
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

## How to compile

_See [Elle: How to compile](https://github.com/infinit/elle#how-to-compile)._

```bash
./drake //elle/build -j 2
```

## Maintainers

 * Website: https://infinit.sh/open-source
 * Email: open+elle@infinit.sh
