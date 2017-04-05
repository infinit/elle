# Elle, *the coroutine-based asynchronous C++ development framework.*

<img src="docs/static_files/elle_logotype@2x.png" alt="Logo - Elle" title="Elle logotype" width="300" style="max-width:300px;">

Elle is a collection of libraries, written in modern C++ (C++14). It contains a rich set of highly reusable concepts, algorithms, API wrappers, ...

Elle is split into different specialized sub-libraries to provide elegant ways to approach asynchronism (using [coroutines](https://en.wikipedia.org/wiki/Coroutine)), networking, formatting, serialization, logging, [RPCs](https://en.wikipedia.org/wiki/Remote_procedure_call), etc.

> *Notes:*
> - Elle is under development, used and maintained by [Infinit](https://infinit.sh) as a core library. APIs, structures and concepts may change over time. You can use it as is but we don't guarantee any API backward compatibility.
> - Elle has a sub-library also called elle, which might change name in a near future.

## Example

Here is an example showing an asynchronous HTTP operation in a natural form (no callbacks) and basic JSON serialization.

```cpp
// Initialize the HTTP Request.
elle::reactor::http::Request r("https://en.wikipedia.org/w/api.php",
                               elle::reactor::http::Method::GET);
r.query_string({
  {"format", "json"},
  {"action", "query"},
  {"prop", "extracts"},
  {"explaintext", ""},
  {"exintro", ""},
  {"titles", "JSON"}
});
// Perform the HTTP request and yield until response is available.
r.finalize();
// Deserialize the json response.
std::cout << elle::json::pretty_print(elle::json::read(r)) << std::endl;
```
<!-- Full example [here](elle/src/elle/examples/samples/get_wikipedia.cc). -->

## Getting Elle.

To download the source code and build Elle by yourself, get it from GitHub.

```bash
git clone https://github.com/infinit/elle --recursive # Clone elle and its submodules.
```

> *Note:* If you cloned it using the GitHub "clone" button, do not forget to run `git submodules update --init --recursive`!

## Structure

As mentioned earlier, Elle is composed of a set of sub-libraries, designed to ease C++ development through robust and flexible implementations, including:
- [elle](src/elle): Utilities including serialization, logs, buffer, formatting, ...
- [reactor](src/elle/reactor): An asynchronous framework using a coroutines scheduler
- [cryptography](src/elle/cryptography): Object-oriented cryptography wrapper around OpenSSL
- [protocol](src/elle/protocol): Network communication designed to support RPCs
- [das](src/elle/das): Symbol-based introspection
- [athena](src/elle/athena): Byzantine environment algorithms (Paxos)
- service/[aws](src/elle/service/aws): *reactorified* AWS API wrapper
- service/[dropbox](src/elle/service/dropbox): *reactorified* Dropbox API wrapper

## How to build Elle (or parts of Elle)

### Requirements

- [gcc](https://gcc.gnu.org) (>= 4.9.2) or [clang](http://clang.llvm.org) (>= 3.5.0) or [mingw](http://mingw.org) (>= 5.3.0).
- [python3](https://www.python.org/download) (>= 3.4.0) and [pip3](https://pip.pypa.io/en/stable).

### Build system

Elle uses [Drake](https://github.com/infinit/drake) and has it as a submodule.

### How to compile

For a detailed procedure, visit our [wiki: How to build](https://github.com/infinit/elle/wiki/How-to-build).

First you need to install drakes requirements.

```bash
sudo pip3 install elle/drake/requirements.txt # Install Drake dependencies.
```
> *Note:* If you don't want Drake dependencies to be installed system-wide, you should consider using [virtualenv](https://virtualenv.pypa.io/en/stable/installation).

Change directory to `elle/_build/<architecture>` where you can find a generic Drake [configuration script](https://github.com/infinit/drake#basic-structures-of-a-drakefile-and-a-drake-script).

#### GNU/Linux


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
./drake //src/elle/cryptography/build -j 2 # To build libcryptography and its dependencies.
./drake //src/elle/reactor/build -j 2 # To build the libreactor and its dependencies.
./drake //src/elle/protocol/build -j 2 # To build the libprotocol and its dependencies.
./drake //...
```
It will result on `<module>/lib/libelle_<module>.so` and its dependencies on GNU/Linux, `<module>/lib/lib<module>.dylib` on macOS, ...

## Dependencies

Elle depends on a few libraries which are automatically downloaded and built for your system by Drake if needed.

- [Boost](http://boost.org) (v1.60.0, _patched_)
- [PatchELF](http://nixos.org/patchelf.html) (v0.9)
- [zlib](http://www.zlib.net) (v1.2.11)
- [OpenSSL](https://www.openssl.org) (v1.0.2g)
- [curl](https://curl.haxx.se) (v7.48.0, patched)
- [libarchive](http://www.libarchive.org) (v3.1.2, _patched_)
- [OpenLDAP](http://www.openldap.org) (v2.4.44)

## List of projects using Elle

- [Infinit](https://github.com/infinit/infinit)

## Wiki

Consult [Elle's wiki](https://github.com/infinit/elle/wiki) for more information about Elle.

## Maintainers

 * Website: https://infinit.sh/open-source
 * Email: open+elle@infinit.sh
