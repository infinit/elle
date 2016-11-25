Cryptography
============

The Cryptography library allows for performing a variety of cryptographic operations from key generation, encryption, decryption, signature, verification to higher-level mechanisms.

Note that the Cryptography library is mostly buffer-oriented even though some operations are allowed on streams such a symmetric encryption, hashing and more.

Sources
-------

The Cryptography library is provided as part of the [**Elle**](http://open.infinit.io/elle) library which is currently unavailable to the public.

Examples
--------

Please refer to the `sample.cc` file which contains several examples related to different cryptographic operations allowed by the library.

Environment variables
---------------------

The Cryptography library can be configured through the following environment variables:

  o INFINIT_CRYPTOGRAPHY_RANDOM_SOURCE defines the path to the source file from which data will be read in order to initialize the pseudo-random generator. The default value for this variable is: _/dev/random_.
  o INFINIT_CRYPTOGRAPHY_ROTATION activates the key rotation mechanism allowing one to derive RSA keys from a seed in a deterministic way.

Dependencies
------------

The Cryptography library relies upon the following libraries:

 * [**OpenSSL**](http://www.openssl.org) provides the fundamental cryptographic operations.
 * [**dOpenSSL**](http://open.infinit.io/dopenssl) allows for deterministic cryptographic operations.
 * [**Elle**](http://open.infinit.io/elle) offers a set of fundamental functionalities from buffer to memory management and serialization.

Maintainers
-----------

 * Website: http://open.infinit.io
 * Email: contact@open.infinit.io
