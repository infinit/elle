# elle, the Infinit utility libraries

elle is a set of portable libraries, written in modern C++. It contains a rich set of highly reusable concepts, algorithms, API wrappers, ...

elle is split into different smaller specialized libraries to provide elegant ways to approach [coroutine](https://en.wikipedia.org/wiki/Coroutine), networking, formatting, serialization, logging, [RPCs](https://en.wikipedia.org/wiki/Remote_procedure_call)...

## Strucure

As mention earlier, elle is a set of libraries, designed to ease C++ development through robusts and flexible implementations, including:
- [elle](https://github.com/infinit/elle/tree/master/elle): Utilities including serialization, logs, buffer, formatting, ...
- [reactor](https://github.com/infinit/elle/tree/master/reactor): An asynchronous framework using a coroutines scheduler
- [cryptography](https://github.com/infinit/elle/tree/master/cryptography): Object-oriented cryptography wrapper around OpenSSL
- [protocol](https://github.com/infinit/elle/tree/master/protocol): Network communication library (RCPs))
- [das](https://github.com/infinit/elle/tree/master/das): Model manipulator and generator
- [athena](https://github.com/infinit/elle/tree/master/athena): Bizantine environment library
- [aws](https://github.com/infinit/elle/tree/master/aws): reactorified AWS API wrapper
- [dropbox](https://github.com/infinit/elle/tree/master/dropbox): reactorified Dropbox API wrapper

> *Notes:*
>   - elle is under development, used and maintained by [infinit](https://infinit.sh) as a set of core libraries. APIs, structures and concepts may change over time. You can use it as is but we don't guaranty any API retrocompatibilty.
>   - elle has a sub-library also called elle, which might change name in a near future.

## Example

```cpp
static const auto keys = elle::cryptography::rsa::keypair::generate(2048);

void
encrypted_hello_world_tcp_server(elle::Buffer const& message = "hello world!")
{
  auto encrypted_message = keys.k().encrypt(message);
  reactor::network::TCPServer server;
  server.listen();
  auto socket = server.accept();
  elle::serialization::json::serialize(*socket, encrypted_message);
}```

## Getting elle.

To download the sources and build elle by yourself, you can just get it from github by running the following commands.

```bash
git clone https://github.com/infinit/elle --recursive # Clone elle and its submodules.
```

## How to build elle (or parts of elle)

### Requirements

- [clang](http://clang.llvm.org) (>= 3.5.0) or [gcc](https://gcc.gnu.org) (>= 4.8.0) or [mingw](http://mingw.org) (>= 5.3.0).
- [python3.4+](https://www.python.org/download/releases/3.0)

### Buildsystem
elle uses the [drake](https://github.com/infinit/drake) buildsystem and carries it as a submodule.
You can find a generic drake configuration script at `_build/<archicture>/drake`.

### How to compile
```bash
sudo pip3 install elle/drake/requirements.txt # Install drake dependencies.
cd elle/_build/<archiecture> (e.g: elle/_build/linux64)
./drake //build -j 2
```
> *Note:* If you don't want the drake dependencies to be installed on your system, you should consider using [virtualenv](https://virtualenv.pypa.io/en/stable/installation).

Because elle has been designed to be modular, you can build specific parts of elle by running `./drake //<module>/build`:

```bash
./drake //cryptography/build -j 2 # To build libcryptography and its dependencies.
./drake //reactor/build -j 2 # To build the libreactor and its dependencies.
./drake //protocol/build -j 2 # To build the libprotocol and its dependencies.
./drake //...
```
It will result on `<module>/lib/lib<module>.so` and its dependencies on linux, `<module>/lib/lib<module>.dylib` on OSX, ...

### Dependencies:

elle depends on a few libraries which are automatically downloaded and built for your system by `drake` if needed.
- [Boost](http://boost.org) (v1.60.0, _patched_)
- [PatchELF](http://nixos.org/patchelf.html) (v0.9)
- [zlib](http://www.zlib.net) (v1.2.8)
- [OpenSSL](https://www.openssl.org) (v1.0.2g)
- [curl](https://curl.haxx.se) (v7.48.0, patched)
- [libarchive](http://www.libarchive.org) (v3.1.2, _patched_)
- [OpenLDAP](http://www.openldap.org) (v2.4.44)

## List of projects using elle:

- [Infinit](https://github.com/infinit/fs)
