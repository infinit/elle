# Elle, *the coroutine-based asynchronous C++ development framework.*

Elle is a collection of libraries, written in modern C++ (C++14). It contains a rich set of highly reusable concepts, algorithms, API wrappers, ...

Elle is split into different smaller specialized libraries to provide elegant ways to approach [coroutine](https://en.wikipedia.org/wiki/Coroutine), networking, formatting, serialization, logging, [RPCs](https://en.wikipedia.org/wiki/Remote_procedure_call), etc.

> *Notes:*
> - Elle is under development, used and maintained by [Infinit](https://infinit.sh) as a set of core libraries. APIs, structures and concepts may change over time. You can use it as is but we don't guarantee any API backward compatibility.
> - Elle has a sub-library also called elle, which might change name in a near future.

## Example
```cpp
reactor::http::Request r("https://en.wikipedia.org/w/api.php",
                         reactor::http::Method::GET);
r.query_string({
  {"format", "json"},
  {"action", "query"},
  {"prop", "extracts"},
  {"explaintext", ""},
  {"exintro", ""},
  {"titles", "JSON"}
});
// Perform the http request and yield until response is available.
r.finalize();
// Deserialize the json response.
std::cout << elle::json::pretty_print(elle::json::read(r)) << std::endl;
```
Full example [here](examples/samples/get_wikipedia.cc).

## Getting Elle.

To download the sources and build Elle by yourself, you can just get it from GitHub by running the following commands.

```bash
git clone https://github.com/infinit/elle --recursive # Clone elle and its submodules.
```

## Structure

As mentioned earlier, Elle is a set of libraries, designed to ease C++ development through robust and flexible implementations, including:
- [elle](elle): Utilities including serialization, logs, buffer, formatting, ...
- [reactor](reactor): An asynchronous framework using a coroutines scheduler
- [cryptography](cryptography): Object-oriented cryptography wrapper around OpenSSL
- [protocol](protocol): Network communication library (RPCs)
- [das](das): Model manipulator and generator
- [athena](athena): Byzantine environment library
- [aws](aws): *reactorified* AWS API wrapper
- [dropbox](dropbox): *reactorified* Dropbox API wrapper

## How to build Elle (or parts of Elle)

### Requirements

- [gcc](https://gcc.gnu.org) (>= 4.9.2) or [clang](http://clang.llvm.org) (>= 3.5.0) or [mingw](http://mingw.org) (>= 5.3.0).
- [python3](https://www.python.org/download) (>= 3.4.0)

### Build system

Elle uses [Drake](https://github.com/infinit/drake) and has it as a submodule.

### How to compile

First you need to install drakes requirements.

```bash
sudo pip3 install elle/drake/requirements.txt # Install Drake dependencies.
```
> *Note:* If you don't want Drake dependencies to be installed on your system, you should consider using [virtualenv](https://virtualenv.pypa.io/en/stable/installation).

Then, change directory to `_build/<architecture>` where you can find a generic Drake configuration script.

#### Linux


```bash
cd elle/_build/linux64
./drake //build -j 2 # Build all libraries using 2 jobs.
```

#### macOS

```bash
cd elle/_build/osx
./drake //build -j 2 # Build all libraries using 2 jobs.
```

Because Elle was designed to be modular, you can build specific parts of Elle by running `./drake //<module>/build`:

```bash
./drake //cryptography/build -j 2 # To build libcryptography and its dependencies.
./drake //reactor/build -j 2 # To build the libreactor and its dependencies.
./drake //protocol/build -j 2 # To build the libprotocol and its dependencies.
./drake //...
```
It will result on `<module>/lib/lib<module>.so` and its dependencies on Linux, `<module>/lib/lib<module>.dylib` on macOS, ...

### Dependencies

Elle depends on a few libraries which are automatically downloaded and built for your system by Drake if needed.

- [Boost](http://boost.org) (v1.60.0, _patched_)
- [PatchELF](http://nixos.org/patchelf.html) (v0.9)
- [zlib](http://www.zlib.net) (v1.2.8)
- [OpenSSL](https://www.openssl.org) (v1.0.2g)
- [curl](https://curl.haxx.se) (v7.48.0, patched)
- [libarchive](http://www.libarchive.org) (v3.1.2, _patched_)
- [OpenLDAP](http://www.openldap.org) (v2.4.44)

## List of projects using Elle

- [Infinit](https://github.com/infinit/infinit)

## Maintainers

 * Website: https://infinit.sh/open-source
 * Email: open+elle@infinit.sh
