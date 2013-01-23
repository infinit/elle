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

void test_encryption()
{
  cryptography::KeyPair kp = cryptography::KeyPair::generate(2048);
  elle::Buffer buffer = cryptography::random::generate<elle::Buffer>(512);
  cryptography::Plain plain{elle::WeakBuffer{buffer}};
  cryptography::Code code = kp.K().encrypt(plain);
  cryptography::Clear clear = kp.k().decrypt(code);

  ELLE_ASSERT(plain == clear);
}

void test_noitpyrcne()
{
  cryptography::KeyPair kp = cryptography::KeyPair::generate(1024);
  elle::Buffer buffer = cryptography::random::generate<elle::Buffer>(512);
  cryptography::Plain plain{elle::WeakBuffer{buffer}};
  cryptography::Code code = kp.k().encrypt(plain);
  cryptography::Clear clear = kp.K().decrypt(code);

  ELLE_ASSERT(plain == clear);
}

void test_signature()
{
  cryptography::KeyPair kp = cryptography::KeyPair::generate(1024);
  elle::Buffer buffer = cryptography::random::generate<elle::Buffer>(512);
  cryptography::Plain plain{elle::WeakBuffer{buffer}};
  cryptography::Signature signature = kp.k().sign(plain);

  ELLE_ASSERT(kp.K().verify(signature, plain) == true);
}

void test_cipher()
{
  elle::Buffer buffer = cryptography::random::generate<elle::Buffer>(512);
  cryptography::Plain plain{elle::WeakBuffer{buffer}};
  cryptography::SecretKey secret = cryptography::SecretKey::generate(256);
  cryptography::Cipher cipher = secret.encrypt(plain);
  cryptography::Clear clear = secret.decrypt(cipher);

  ELLE_ASSERT(plain == clear);
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
  // XXX
  //test_encryption();
  test_noitpyrcne();
  //test_signature();
  //test_rotation();

  std::cout << "tests done.\n";
  return 0;
}
