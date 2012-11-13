#include <elle/types.hh>
#include <elle/Buffer.hh>

#include <cryptography/PrivateKey.hh>
#include <cryptography/PublicKey.hh>
#include <cryptography/KeyPair.hh>
#include <cryptography/random.hh>
#include <cryptography/SecretKey.hh>
#include <cryptography/Cipher.hh>
#include <cryptography/Plain.hh>
#include <cryptography/Code.hh>
#include <cryptography/Clear.hh>
#include <cryptography/Signature.hh>
// XXX[temporary: for cryptography]
using namespace infinit;

#include <iostream>

#define CHECK(call) if (call != elle::Status::Ok) { assert(false); } else

void test_encryption()
{
  cryptography::KeyPair kp{cryptography::KeyPair::generate(2048)};
  cryptography::PublicKey K;
  cryptography::PrivateKey k;
  cryptography::Code code;

  elle::Buffer buffer{cryptography::random::generate<elle::Buffer>(512)};
  cryptography::Plain plain{elle::WeakBuffer{buffer}};

  K = kp.K();
  k = kp.k();

  CHECK(K.Encrypt(plain, code));

  cryptography::Clear clear{k.decrypt<cryptography::Clear>(code)};

  assert(plain == clear);
}

void test_noitpyrcne()
{
  cryptography::KeyPair kp{cryptography::KeyPair::generate(4096)};
  cryptography::PublicKey K;
  cryptography::PrivateKey k;
  cryptography::Code code;
  cryptography::Clear clear;

  elle::Buffer buffer{cryptography::random::generate<elle::Buffer>(512)};
  cryptography::Plain plain{elle::WeakBuffer{buffer}};

  K = kp.K();
  k = kp.k();

  CHECK(k.Encrypt(plain, code));

  CHECK(K.Decrypt(code, clear));

  assert(plain == clear);
}

void test_signature()
{
  cryptography::KeyPair kp{cryptography::KeyPair::generate(1024)};
  cryptography::PublicKey K;
  cryptography::PrivateKey k;
  cryptography::Signature signature;

  elle::Buffer buffer{cryptography::random::generate<elle::Buffer>(512)};
  cryptography::Plain plain{elle::WeakBuffer{buffer}};

  K = kp.K();
  k = kp.k();

  signature = k.sign(plain);

  CHECK(K.Verify(signature, plain));
}

void test_cipher()
{
  cryptography::SecretKey secret;
  cryptography::Cipher cipher;
  cryptography::Clear clear;

  elle::Buffer buffer{cryptography::random::generate<elle::Buffer>(512)};
  cryptography::Plain plain{elle::WeakBuffer{buffer}};

  CHECK(secret.Generate(256));

  CHECK(secret.Encrypt(plain, cipher));

  CHECK(secret.Decrypt(cipher, clear));

  assert(plain == clear);
}

void test_rotation()
{
                /* XXX
                Seed            seed;
                KeyPair         initial;
                Natural32       rotations;
                Natural32       i;
                Seed            s;
                KeyPair         p;
                PrivateKey      k;
                PublicKey       K;

                // generate the initial seed.
                if (seed.generate() == Status::Error)
                  escape("unable to generate the seed");

                // rotate the key pair once in order to be truly random
                // i.e without a small exponent 'e' such as 3.
                if (kp->Rotate(seed, initial) == Status::Error)
                  escape("unable to rotate the initial key pair");

                // generate a random number of rotations to apply.
                rotations = random::generate(Test::MinimumKeyRotations,
                                             Test::MaximumKeyRotations);

                // assign the current seed and key pair.
                s = seed;
                p = initial;

                // perform the key rotations.
                for (i = 0; i < rotations; i++)
                  {
                    Seed        _s;
                    KeyPair     _p;

                    // rotate the seed.
                    if (s.Rotate(kp->k, _s) == Status::Error)
                      escape("unable to rotate the seed");

                    // assign the seed.
                    s = _s;

                    // rotate the key pair.
                    if (p.Rotate(s, _p) == Status::Error)
                      escape("unable to rotate the key pair");

                    // assign the key pair.
                    p = _p;
                  }

                // assign the necessary for the derivation.
                s = s;
                k = initial.k;

                // perform the key derivations.
                for (i = 0; i < rotations; i++)
                  {
                    Seed        _s;

                    // derive the seed.
                    if (s.Derive(kp->K, _s) == Status::Error)
                      escape("unable to derive the seed");

                    // assign the seed.
                    s = _s;
                  }

                // derive the key from the seed.
                if (k.Derive(s, K) == Status::Error)
                  escape("unable to derive the key");

                // finally, compare the public and private key
                // with the initial.
                if ((initial.K != K) || (initial.k != k))
                  escape("the keys differ from the initial");
                */
}

int main()
{
  test_encryption();
  test_noitpyrcne();
  test_signature();
  test_rotation();

  std::cout << "tests done.\n";
  return 0;
}
