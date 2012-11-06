#include <elle/types.hh>
#include <elle/Buffer.hh>
#include <elle/cryptography/PrivateKey.hh>
#include <elle/cryptography/PublicKey.hh>
#include <elle/cryptography/KeyPair.hh>
#include <elle/cryptography/random.hh>
#include <elle/cryptography/SecretKey.hh>
#include <elle/cryptography/Cipher.hh>
#include <elle/cryptography/Plain.hh>
#include <elle/cryptography/Code.hh>
#include <elle/cryptography/Clear.hh>
#include <elle/cryptography/Signature.hh>

#include <iostream>

#define CHECK(call) if (call != elle::Status::Ok) { assert(false); } else

void test_encryption()
{
  elle::cryptography::KeyPair kp{elle::cryptography::KeyPair::generate(2048)};
  elle::cryptography::PublicKey K;
  elle::cryptography::PrivateKey k;
  elle::cryptography::Code code;
  elle::cryptography::Clear clear;

  elle::Buffer buffer{elle::cryptography::random::generate<elle::Buffer>(512)};
  elle::cryptography::Plain plain{elle::WeakBuffer{buffer}};

  K = kp.K;
  k = kp.k;

  CHECK(K.Encrypt(plain, code));

  CHECK(k.Decrypt(code, clear));

  assert(plain == clear);
}

void test_noitpyrcne()
{
  elle::cryptography::KeyPair kp{elle::cryptography::KeyPair::generate(4096)};
  elle::cryptography::PublicKey K;
  elle::cryptography::PrivateKey k;
  elle::cryptography::Code code;
  elle::cryptography::Clear clear;

  elle::Buffer buffer{elle::cryptography::random::generate<elle::Buffer>(512)};
  elle::cryptography::Plain plain{elle::WeakBuffer{buffer}};

  K = kp.K;
  k = kp.k;

  CHECK(k.Encrypt(plain, code));

  CHECK(K.Decrypt(code, clear));

  assert(plain == clear);
}

void test_signature()
{
  elle::cryptography::KeyPair kp{elle::cryptography::KeyPair::generate(1024)};
  elle::cryptography::PublicKey K;
  elle::cryptography::PrivateKey k;
  elle::cryptography::Signature signature;

  elle::Buffer buffer{elle::cryptography::random::generate<elle::Buffer>(512)};
  elle::cryptography::Plain plain{elle::WeakBuffer{buffer}};

  K = kp.K;
  k = kp.k;

  signature = k.sign(plain);

  CHECK(K.Verify(signature, plain));
}

void test_cipher()
{
  elle::cryptography::SecretKey secret;
  elle::cryptography::Cipher cipher;
  elle::cryptography::Clear clear;

  elle::Buffer buffer{elle::cryptography::random::generate<elle::Buffer>(512)};
  elle::cryptography::Plain plain{elle::WeakBuffer{buffer}};

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
