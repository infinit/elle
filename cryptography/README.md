# Cryptography

The Cryptography library allows for performing a variety of cryptographic operations from key generation, encryption, decryption, signature, verification to higher-level mechanisms.

Note that the Cryptography library is mostly buffer-oriented even though some operations are allowed on streams such a symmetric encryption, hashing and more.

Cryptoraphy is part of the [Elle](https://github.com/infinit/elle) set of libraries.

## Motivation

Cryptography is built on top of [OpenSSL](http://www.openssl.org) which provides the fundamental cryptographic operations. Motivation behind Cryptography is to wrap OpenSSL (written in C), in order to ease integration in a C++ project. It provides an elegant and object-oriented API and follows the RAII idiom to lazily simplify initialization and cleanup.

## Examples

```cpp
// Sign/verify with DSA.
{
  std::string data("Data to sign...");

  // Generate a random dsa KeyPair.
  infinit::cryptography::dsa::KeyPair keypair =
    infinit::cryptography::dsa::keypair::generate(1024);

  // Create a signature for data (with k: the private key).
  elle::Buffer signature = keypair.k().sign(data);

  // Ensure verify the signature (with K: the public key).
  assert(keypair.K().verify(signature, data) == true));
}
```
Please refer to the [sample.cc]("sample.cc") file which contains several examples related to different cryptographic operations allowed by the library.

## Environment variables

The Cryptography library can be configured through the following environment variables:

* **INFINIT_CRYPTOGRAPHY_RANDOM_SOURCE** defines the path to the source file from which data will be read in order to initialize the pseudo-random generator. The default value for this variable is: _/dev/random_.
* **INFINIT_CRYPTOGRAPHY_ROTATION activates** the key rotation mechanism allowing one to derive RSA keys from a seed in a deterministic way.

## Dependencies

The Cryptography library relies upon the following libraries:

 * [**OpenSSL**](http://www.openssl.org) provides the fundamental cryptographic operations.
 * [**dOpenSSL**](http://open.infinit.io/dopenssl) allows for deterministic cryptographic operations.
 * [**Elle**](http://open.infinit.io/elle) offers a set of fundamental functionalities from buffer to memory management and serialization.

Maintainers
-----------

 * Website: http://open.infinit.io
 * Email: contact@open.infinit.io
