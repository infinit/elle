//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [wed jan 28 11:22:24 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include "Test.hh"

namespace elle
{
  namespace test
  {

//
// ---------- definitions -----------------------------------------------------
//

    const Natural32             Test::MinimumTestsNumber = 1;
    const Natural32             Test::MaximumTestsNumber = 3456;

    const Natural32             Test::MinimumKeyLength = 1024;
    const Natural32             Test::MaximumKeyLength = 2048;

    const Natural32             Test::MinimumKeyRotations = 1;
    const Natural32             Test::MaximumKeyRotations = 10;

//
// ---------- functions -------------------------------------------------------
//

    ///
    /// this method represents the test's entry point.
    ///
    Status              Main()
    {
      Natural32         number;
      KeyPair*          kp;
      SecretKey*        sk;
      Natural32         i;

      // init the library.
      if (Elle::Initialize() == Status::Error)
        escape("unable to initialize the Elle library");

      // allocate an initialize key pair.
      kp = new KeyPair;

      // generate a key pair
      if (kp->Generate(Random::Generate(Test::MinimumKeyLength,
                                        Test::MaximumKeyLength)) ==
          Status::Error)
        escape("unable to generate the initial key pair");

      // allocate a secret key.
      sk = new SecretKey;

      // generate the secret key.
      if (sk->Generate(Random::Generate(Test::MinimumKeyLength,
                                        Test::MaximumKeyLength)) ==
          Status::Error)
        escape("unable to generate the initial secret key");

      // compute the number of tests.
      number = Random::Generate(Test::MinimumTestsNumber,
                                Test::MaximumTestsNumber);

      // for every test.
      for (i = 0; i < number; i++)
        {
          Byte          type = Random::Generate(Test::TypeKeys,
                                                Test::TypeCipher);

          printf("%u/%u\n", i, number);

          // according to the type of test: generating new keys,
          // encrypting/decrypting, signing/verifying or
          // cipher/de-ciphering...
          switch (type)
            {
            case Test::TypeKeys:
              {
                // delete the current keys.
                delete kp;
                delete sk;

                // allocate the new keys
                kp = new KeyPair;
                sk = new SecretKey;

                // generate the new keys.
                if (kp->Generate(Random::Generate(Test::MinimumKeyLength,
                                                  Test::MaximumKeyLength)) ==
                    Status::Error)
                  escape("unable to generate a new key pair");

                if (sk->Generate(Random::Generate(Test::MinimumKeyLength,
                                                  Test::MaximumKeyLength)) ==
                    Status::Error)
                  escape("unable to generate a secret key");

                break;
              }
            case Test::TypeEncryption:
              {
                PublicKey       K;
                PrivateKey      k;
                Plain           plain;
                Code            code;
                Clear           clear;

                // test the key assignment.
                K = kp->K;
                k = kp->k;

                // generate an input.
                if (Random::Generate(plain) == Status::Error)
                  escape("unable to generate a plain");

                // encrypt the input.
                if (K.Encrypt(plain, code) == Status::Error)
                  escape("unable to encrypt the plain");

                // decrypt it.
                if (k.Decrypt(code, clear) == Status::Error)
                  escape("unable to decrypt the code");

                // compare the input and output.
                if (plain != clear)
                  escape("the clear differs from the plain");

                break;
              }
            case Test::TypeNoitpyrcne:
              {
                PublicKey       K;
                PrivateKey      k;
                Plain           plain;
                Code            code;
                Clear           clear;

                // test the key assignment.
                K = kp->K;
                k = kp->k;

                // generate an input.
                if (Random::Generate(plain) == Status::Error)
                  escape("unable to generate a plain");

                // encrypt the input with the private key.
                if (k.Encrypt(plain, code) == Status::Error)
                  escape("unable to encrypt the plain");

                // decrypt it with the public key.
                if (K.Decrypt(code, clear) == Status::Error)
                  escape("unable to decrypt the code");

                // compare the input and output.
                if (plain != clear)
                  escape("the clear differs from the plain");

                break;
              }
            case Test::TypeSignature:
              {
                PublicKey       K;
                PrivateKey      k;
                Plain           plain;
                Signature       signature;

                // test the key assignment.
                K = kp->K;
                k = kp->k;

                // generate an input.
                if (Random::Generate(plain) == Status::Error)
                  escape("unable to generate a plain");

                // sign the plain.
                if (k.Sign(plain, signature) == Status::Error)
                  escape("unable to sign the plain");

                // verify the signature.
                if (K.Verify(signature, plain) == Status::Error)
                  escape("unable to verify the signature or "
                         "the signature differs from the plain");

                break;
              }
            case Test::TypeCipher:
              {
                SecretKey       s;
                Plain           plain;
                Cipher          cipher;
                Clear           clear;

                // test the key assignment.
                s = *sk;

                // generate a plain.
                if (Random::Generate(plain) == Status::Error)
                  escape("unable to generate a plain");

                // cipher the plain.
                if (s.Encrypt(plain, cipher) == Status::Error)
                  escape("unable to cipher the plain");

                // decypher the code.
                if (s.Decrypt(cipher, clear) == Status::Error)
                  escape("unable to cipher the code");

                // compare the clear with the initial plain.
                if (plain != clear)
                  escape("the clear differs from the plain");

                break;
              }
            case Test::TypeRotation:
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
                if (seed.Generate() == Status::Error)
                  escape("unable to generate the seed");

                // rotate the key pair once in order to be truly random
                // i.e without a small exponent 'e' such as 3.
                if (kp->Rotate(seed, initial) == Status::Error)
                  escape("unable to rotate the initial key pair");

                // generate a random number of rotations to apply.
                rotations = Random::Generate(Test::MinimumKeyRotations,
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

                break;
              }
            }
        }

      // delete the current keys.
      delete kp;
      delete sk;

      // clean the library.
      if (Elle::Clean() == Status::Error)
        escape("unable to clean the Elle library");

      return Status::Ok;
    }

  }
}

//
// ---------- main ------------------------------------------------------------
//

int                     main()
{
  if (elle::test::Main() == elle::Status::Error)
    {
      show();

      return (1);
    }

  std::cout << "[success]" << std::endl;

  return (0);
}
