# Elle, the Infinit utility libraries

Elle is a set of portable libraries, written in modern C++. It contains a rich set of highly reusable concepts, algorithms, API wrappers, ...

Elle is split into different smaller specialized libraries to provide elegant ways to approach [coroutine](https://en.wikipedia.org/wiki/Coroutine), networking, formatting, serialization, logging, [RPCs](https://en.wikipedia.org/wiki/Remote_procedure_call)...

## Strucure

As mentioned earlier, Elle is a set of libraries, designed to ease C++ development through robusts and flexible implementations, including:
- [elle](elle): Utilities including serialization, logs, buffer, formatting, ...
- [reactor](reactor): An asynchronous framework using a coroutines scheduler
- [cryptography](cryptography): Object-oriented cryptography wrapper around OpenSSL
- [protocol](protocol): Network communication library (RCPs))
- [das](das): Model manipulator and generator
- [athena](athena): Bizantine environment library
- [aws](aws): *reactorified* AWS API wrapper
- [dropbox](dropbox): *reactorified* Dropbox API wrapper

> *Notes:*
> - Elle is under development, used and maintained by [infinit](https://infinit.sh) as a set of core libraries. APIs, structures and concepts may change over time. You can use it as is but we don't guaranty any API backwards compatibility.
> - Elle has a sub-library also called elle, which might change name in a near future.

## Getting Elle.

To download the sources and build Elle by yourself, you can just get it from github by running the following commands.

```bash
git clone https://github.com/infinit/elle --recursive # Clone elle and its submodules.
```

## How to build Elle (or parts of Elle)

### Requirements

-  [gcc](https://gcc.gnu.org) (>= 4.9.2) or [clang](http://clang.llvm.org) (>= 3.5.0) or [mingw](http://mingw.org) (>= 5.3.0).
- [python3](https://www.python.org/download/releases/3.0) (>= 3.4.0)

### Build system

Elle uses [Drake](https://github.com/infinit/drake) and has it as a submodule.

You can find a generic Drake configuration script at `_build/<architecture>/drake`.

### How to compile
```bash
sudo pip3 install elle/drake/requirements.txt # Install Drake dependencies.
cd elle/_build/<architecture> # (e.g: elle/_build/linux64)
./drake //build -j 2 # Build all libraries using 2 jobs.
```
> *Note:* If you don't want Drake dependencies to be installed on your system, you should consider using [virtualenv](https://virtualenv.pypa.io/en/stable/installation).

Because Elle has been designed to be modular, you can build specific parts of Elle by running `./drake //<module>/build`:

```bash
./drake //cryptography/build -j 2 # To build libcryptography and its dependencies.
./drake //reactor/build -j 2 # To build the libreactor and its dependencies.
./drake //protocol/build -j 2 # To build the libprotocol and its dependencies.
./drake //...
```
It will result on `<module>/lib/lib<module>.so` and its dependencies on Linux, `<module>/lib/lib<module>.dylib` on MacOS, ...

## Example

The following example shows how to use:
* das to declare models
* reactor to spawn coroutines and get a resource via http
* elle::serialization to serialize to both json and binary
* cryptography to manipulate rsa::KeyPair, verify signature and decrypt a blob of data

```cpp
#include <das/Symbol.hh>
#include <das/model.hh>
#include <das/printer.hh>
#include <das/serializer.hh>

#include <elle/AtomicFile.hh>
#include <elle/serialization/binary.hh>
#include <elle/serialization/json.hh>

#include <cryptography/rsa/KeyPair.hh>

#include <reactor/http/Request.hh>
#include <reactor/http/exceptions.hh>
#include <reactor/scheduler.hh>

// Declare symbols used by das.
DAS_SYMBOL(array);
DAS_SYMBOL(dict);
DAS_SYMBOL(f);
DAS_SYMBOL(i);
DAS_SYMBOL(encrypted_payload);
DAS_SYMBOL(body);
DAS_SYMBOL(signature);

// Create an example data that look like that:
// {
//   "body": {
//     "array": [...],
//     "dict": {...},
//     "f": ...,
//     "i": ...,
//     "encrypted_payload": ...
//   }
// }
struct Body
{
  std::vector<std::string> array;
  std::unordered_map<std::string, std::string> dict;
  double f;
  int i;
  elle::Buffer encrypted_payload;

  // Generate a model for the class Body.
  using Model = das::Model<Body, elle::meta::List<Symbol_array,
                                                  Symbol_dict,
                                                  Symbol_f,
                                                  Symbol_i,
                                                  Symbol_encrypted_payload>>;
};

// Create a wrapper around any class to have:
// {
//   body: {...}
//   signature: ...
// }
template <typename T>
struct Signed
{
  T body;
  elle::Buffer signature;

  // What to sign.
  // Here, we use the binary representation of body.
  static
  elle::Buffer
  signature_plain(T const& body)
  {
    elle::Buffer res;
    {
      elle::IOStream output(res.ostreambuf());
      elle::serialization::binary::SerializerOut serializer(output, false);
      das::serialize(body, serializer);
    }
    return res;
  }

  // Generate a Model the templated class Signed<T>.
  using Model = das::Model<Signed<T>, elle::meta::List<Symbol_body,
                                                       Symbol_signature
                                                       >>;
};

using Example = Signed<Body>;

// Declare both Body and Example serializable.
DAS_SERIALIZE(Body);
DAS_SERIALIZE(Example);

// Declare all das::Model printable.
using das::operator <<;

// Read your fake keypair from examples/example.key.
static
infinit::cryptography::rsa::KeyPair
key()
{
  elle::AtomicFile f("examples/example.key");
  return f.read() << [&] (elle::AtomicFile::Read& read)
  {
    return elle::serialization::json::deserialize<
      infinit::cryptography::rsa::KeyPair>(read.stream(), false);
  };
}

// Download the example file.
static
Example
get()
{
  std::string url = "https://gist.githubusercontent.com"
    "/Dimrok"
    "/a9a6898bc84397b7b037b97a03c58437/raw"
    "/d7764d2b26814af3bfff17c707dc4f33a93f7e8c"
    "/example.json";
  std::cout << "1. GET example.json at " << url << std::endl;
  reactor::http::Request r(url);
  reactor::wait(r);
  std::cout << "2. Deserialize response" << std::endl;
  return elle::serialization::json::deserialize<Example>(r);
}

// 1. Downlaod the example file
// 2. Deserialize its content
// 3. Ensure the signature is valid (the private key has signed the body)
// 4. Decrypt the hidden secret
int
main()
{
  reactor::Scheduler sched;
  reactor::Thread t(
    sched, "main",
    []
    {
      try
      {
        auto example = get();
        auto public_key = key().K();
        // Check if you are the author of the signature.
        std::cout << "3. Verify signature" << std::endl;
        if (!public_key.verify(example.signature,
                               Example::signature_plain(example.body)))
          // Throw an elle::Exception.
          elle::err("invalid signature: Someone replaced your data");
        std::cout << "> You are the author of the of "
                 << example.body << std::endl;
        std::cout << "4. Decrypt secret" << std::endl;
        elle::ConstWeakBuffer payload(example.body.encrypted_payload);
        auto secret = key().k().decrypt(payload).string();
        // std::cout << "> " << secret << std::endl;
      }
      catch (reactor::http::ResolutionFailure const& e)
      {
        std::cerr << "Unable to resolve " << e.url() << std::endl;
        throw;
      }
      catch (reactor::http::RequestError const& e)
      {
        std::cerr << "Unable to get " << e.url() << std::endl;
        throw;
      }
    });
  try
  {
    sched.run();
  }
  catch (...)
  {
    return 1;
  }
  return 0;
}
```

### Dependencies:

Elle depends on a few libraries which are automatically downloaded and built for your system by Drake if needed.

- [Boost](http://boost.org) (v1.60.0, _patched_)
- [PatchELF](http://nixos.org/patchelf.html) (v0.9)
- [zlib](http://www.zlib.net) (v1.2.8)
- [OpenSSL](https://www.openssl.org) (v1.0.2g)
- [curl](https://curl.haxx.se) (v7.48.0, patched)
- [libarchive](http://www.libarchive.org) (v3.1.2, _patched_)
- [OpenLDAP](http://www.openldap.org) (v2.4.44)

## List of projects using Elle:

- [Infinit](https://github.com/infinit/fs)
