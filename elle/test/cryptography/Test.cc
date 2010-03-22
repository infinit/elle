//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/test/cryptography/Test.cc
//
// created       julien quintard   [wed jan 28 11:22:24 2009]
// updated       julien quintard   [sun mar 21 14:12:48 2010]
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

    const Natural32		Test::MinimumTestsNumber = 1;
    const Natural32		Test::MaximumTestsNumber = 3456;

    const Natural32		Test::MinimumRegionSize = 1;
    const Natural32		Test::MaximumRegionSize = 12345;

    const Natural32		Test::MinimumKeyLength = 1024;
    const Natural32		Test::MaximumKeyLength = 2048;

//
// ---------- methods ---------------------------------------------------------
//

    Status		Test::Generate(Region&			region)
    {
      Natural32		size = Random::Generate(Test::MinimumRegionSize,
						Test::MaximumRegionSize);
      Byte*		buffer;
      Natural32		i;

      enter(slab(buffer, ::free));

      // allocate a buffer.
      if ((buffer = (Byte*)::malloc(size)) == NULL)
	escape("unable to allocate memory");

      // randomize the buffer contents.
      for (i = 0; i < size; i++)
	*(buffer + i) = Random::Generate(elle::core::Type<Character>::Minimum,
					 elle::core::Type<Character>::Maximum);

      // assign the buffer to the region.
      if (region.Acquire(buffer, size) == StatusError)
	escape("unable to assign the buffer to the region");

      // since included in the region.
      waive(buffer);

      leave();
    }

    Status		Test::Main()
    {
      Natural32		number;
      KeyPair*		kp;
      SecretKey*	sk;
      Natural32		i;

      enter(instance(kp),
	    instance(sk));

      // init the library.
      if (Elle::Initialize() == StatusError)
	escape("unable to initialize the Elle library");

      // allocate an initialize key pair.
      kp = new KeyPair;

      // generate a key pair
      if (kp->Generate(Random::Generate(Test::MinimumKeyLength,
					Test::MaximumKeyLength)) == StatusError)
	escape("unable to generate the initial key pair");

      // allocate a secret key.
      sk = new SecretKey;

      // generate the secret key.
      if (sk->Generate(Random::Generate(Test::MinimumKeyLength,
                                        Test::MaximumKeyLength)) == StatusError)
	escape("unable to generate the initial secret key");

      // compute the number of tests.
      number = Random::Generate(Test::MinimumTestsNumber,
				Test::MaximumTestsNumber);

      // for every test.
      for (i = 0; i < number; i++)
	{
	  Byte		type = Random::Generate(Test::TypeKeys,
						Test::TypeCipher);

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
						  Test::MaximumKeyLength)) == StatusError)
		  escape("unable to generate a new key pair");

		if (sk->Generate(Random::Generate(Test::MinimumKeyLength,
						  Test::MaximumKeyLength)) == StatusError)
		  escape("unable to generate a secret key");

		break;
	      }
	    case Test::TypeEncryption:
	      {
		PublicKey	K;
		PrivateKey	k;
		Plain		plain;
		Code		code;
		Clear		clear;

		// test the key assignment.
		K = kp->K;
		k = kp->k;

		// generate an input.
		if (Test::Generate(plain) == StatusError)
		  escape("unable to generate a plain");

		// encrypt the input.
		if (K.Encrypt(plain, code) == StatusError)
		  escape("unable to encrypt the plain");

		// decrypt it.
		if (k.Decrypt(code, clear) == StatusError)
		  escape("unable to decrypt the code");

		// compare the input and output.
		if (plain != clear)
		  escape("the clear differs from the plain");

		break;
	      }
	    case Test::TypeSignature:
	      {
		PublicKey	K;
		PrivateKey	k;
		Plain		plain;
		Signature	signature;

		// test the key assignment.
		K = kp->K;
		k = kp->k;

		// generate an input.
		if (Test::Generate(plain) == StatusError)
		  escape("unable to generate a plain");

		// sign the plain.
		if (k.Sign(plain, signature) == StatusError)
		  escape("unable to sign the plain");

		// verify the signature.
		if (K.Verify(signature, plain) != StatusTrue)
		  escape("unable to verify the signature or "
			 "the signature differs from the plain");

		break;
	      }
	    case Test::TypeCipher:
	      {
		SecretKey	s;
		Plain		plain;
		Cipher		cipher;
		Clear		clear;

		// test the key assignment.
		s = *sk;

		// generate a plain.
		if (Test::Generate(plain) == StatusError)
		  escape("unable to generate a plain");

		// cipher the plain.
		if (s.Encrypt(plain, cipher) == StatusError)
		  escape("unable to cipher the plain");

		// decypher the code.
		if (s.Decrypt(cipher, clear) == StatusError)
		  escape("unable to cipher the code");

		// compare the clear with the initial plain.
		if (plain != clear)
		  escape("the clear differs from the plain");

		break;
	      }
	    }
	}

      // stop tracking.
      waive(kp);
      waive(sk);

      // delete the current keys.
      delete kp;
      delete sk;

      // clean the library.
      if (Elle::Clean() == StatusError)
	escape("unable to clean the Elle library");

      leave();
    }

  }
}

//
// ---------- main ------------------------------------------------------------
//

int			main()
{
  if (elle::test::Test::Main() == elle::misc::StatusError)
    {
      show();

      return (1);
    }

  std::cout << "[success]" << std::endl;

  return (0);
}
