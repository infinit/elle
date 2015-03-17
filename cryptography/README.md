Cryptography
============

The Cryptography library allows for performing a variety of cryptographic operations from key generation, encryption, decryption, signature, verification to higher-level mechanisms like public-key challenges.

Note that the Cryptography library is not stream-oriented and therefore performs cryptographic operations at the buffer level.

Sources
-------

The Cryptography library is provided as part of the [**Elle**](http://open.infinit.io/elle) library which is currently unavailable to the public.

Examples
--------

The following source code can be copied and pasted in a test file, say `test.c`.
This sample shows how the Cryptography library can be used to encrypt/decrypt data very easily without having to manually make sure data does not to exceed the RSA key's modulus for instance.

```C++
#include <cryptography/KeyPair.hh>

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    printf("usage: %s [text]\n", argv[0]);
    return(1);
  }

  infinit::cryptography::KeyPair keypair =
    infinit::cryptography::KeyPair::generate(
      infinit::cryptography::Cryptosystem::rsa,
      1024);

  infinit::cryptography::Code code = keypair.K().encrypt(
    infinit::cryptography::Plain{
      elle::ConstWeakBuffer{argv[1], ::strlen(argv[1])}});

  elle::printf("[code] %x\n", code);

  infinit::cryptography::Clear clear = keypair.k().decrypt(code);

  elle::printf("[clear] %s\n", clear);

  return (0);
}
```

Environment variables
---------------------

The Cryptography library can be configured through the following environment variables:

 * INFINIT_CRYPTOGRAPHY_RANDOM_SOURCE defines the path to the source file from which data will be read in order to initialize the pseudo-random generator. The default value for this variable is: _/dev/random_
 * INFINIT_CRYPTOGRAPHY_ROTATION activates the key rotation mechanism allowing one to derive key pairs, public and private keys from a cryptographic seed.

Dependencies
------------

The Cryptography library relies upon the following libraries:

 * [**OpenSSL**](http://www.openssl.org) provides the fundamental cryptographic operations.
 * [**dOpenSSL**](http://open.infinit.io/dopenssl) allows for deterministic cryptographic operations.
 * [**Elle**]*(http://open.infinit.io/elle) offers a set of fundamental functionalities from buffer to memory management and serialization.

Maintainers
-----------

 * Website: http://open.infinit.io
 * Email: contact@open.infinit.io
