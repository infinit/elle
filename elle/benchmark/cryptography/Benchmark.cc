//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infi...elle/benchmark/cryptography/Benchmark.cc
//
// created       julien quintard   [wed jan 28 11:22:24 2009]
// updated       julien quintard   [sat sep  3 19:55:11 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include "Benchmark.hh"

namespace elle
{
  namespace benchmark
  {

//
// ---------- definitions -----------------------------------------------------
//

    const Natural32		Benchmark::AsymmetricKeyLength = 1024;
    const Natural32		Benchmark::SymmetricKeyLength = 256;

    const Natural32		Benchmark::BlockSize = 4096;

//
// ---------- functions -------------------------------------------------------
//

    ///
    /// this is the benchmark's entry point.
    ///
    Status		Main()
    {
      Byte		buffer[Benchmark::BlockSize];

      enter();

      // init the library.
      if (Elle::Initialize() == StatusError)
	escape("unable to initialize the Elle library");

      // Asymmetric Generation
      {
	KeyPair		keypair;
	Time		time;

	time.Start();

	// generate the new keys.
	if (keypair.Generate(Benchmark::AsymmetricKeyLength) == StatusError)
	  escape("unable to generate a new key pair");

	time.Stop();

	std::cout << "Asymmetric[" << Benchmark::AsymmetricKeyLength
		  << "] Generation: ";
	time.Print();
	std::cout << std::endl;
      }

      // Asymmetric Encryption
      {
	KeyPair		keypair;
	Region		region;
	Code		code;
	Time		time;

	// generate the new keys.
	if (keypair.Generate(Benchmark::AsymmetricKeyLength) == StatusError)
	  escape("unable to generate a new key pair");

	// prepare random data.
	if (region.Wrap(buffer, sizeof (buffer)) == StatusError)
	  escape("unable to prepare the block");

	time.Start();

	// encrypt the input.
	if (keypair.K.Encrypt(region, code) == StatusError)
	  escape("unable to encrypt the block");

	time.Stop();

	std::cout << "Asymmetric[" << Benchmark::AsymmetricKeyLength
		  << "] Block[" << Benchmark::BlockSize << "] Encryption: ";
	time.Print();
	std::cout << std::endl;
      }

      // Asymmetric Decryption
      {
 	KeyPair		keypair;
	Region		region;
	Code		code;
	Clear		clear;
	Time		time;

	// generate the new keys.
	if (keypair.Generate(Benchmark::AsymmetricKeyLength) == StatusError)
	  escape("unable to generate a new key pair");

	// some random data.
	if (region.Wrap(buffer, sizeof (buffer)) == StatusError)
	  escape("unable to prepare the block");

	// encrypt the input.
	if (keypair.K.Encrypt(region, code) == StatusError)
	  escape("unable to encrypt the block");

	time.Start();

	// decrypt the data.
	if (keypair.k.Decrypt(code, clear) == StatusError)
	  escape("unable to decrypt the block");

	time.Stop();

	// compare.
	if (region != clear)
	  escape("the decrypted data is different from the original");

	std::cout << "Asymmetric[" << Benchmark::AsymmetricKeyLength
		  << "] Block[" << Benchmark::BlockSize << "] Decryption: ";
	time.Print();
	std::cout << std::endl;
      }

      // Asymmetric Signature
      {
	KeyPair		keypair;
	Region		region;
	Signature	signature;
	Time		time;

	// generate the new keys.
	if (keypair.Generate(Benchmark::AsymmetricKeyLength) == StatusError)
	  escape("unable to generate a new key pair");

	// prepare random data.
	if (region.Wrap(buffer, sizeof (buffer)) == StatusError)
	  escape("unable to prepare the block");

	time.Start();

	// sign the input.
	if (keypair.k.Sign(region, signature) == StatusError)
	  escape("unable to sign the block");

	time.Stop();

	std::cout << "Asymmetric[" << Benchmark::AsymmetricKeyLength
		  << "] Block" << Benchmark::BlockSize << "] Signature: ";
	time.Print();
	std::cout << std::endl;
      }

      // Asymmetric Verification
      {
	KeyPair		keypair;
	Region		region;
	Signature	signature;
	Time		time;

	// generate the new keys.
	if (keypair.Generate(Benchmark::AsymmetricKeyLength) == StatusError)
	  escape("unable to generate a new key pair");

	// some random data.
	if (region.Wrap(buffer, sizeof (buffer)) == StatusError)
	  escape("unable to prepare the block");

	// sign the input.
	if (keypair.k.Sign(region, signature) == StatusError)
	  escape("unable to sign the block");

	time.Start();

	// verify the data.
	if (keypair.K.Verify(signature, region) == StatusError)
	  escape("unable to verify the signature");

	time.Stop();

	std::cout << "Asymmetric[" << Benchmark::AsymmetricKeyLength
		  << "] Block[" << Benchmark::BlockSize << "] Verification: ";
	time.Print();
	std::cout << std::endl;
      }

      // Hash
      {
	Region		region;
	Digest		digest;
	Time		time;

	// some random data.
	if (region.Wrap(buffer, sizeof (buffer)) == StatusError)
	  escape("unable to prepare the block");

	time.Start();

	// hash the data.
	if (OneWay::Hash(region, digest) == StatusError)
	  escape("unable to hash the data");

	time.Stop();

	std::cout << "Block[" << Benchmark::BlockSize << "] Hash: ";
	time.Print();
	std::cout << std::endl;
      }

      // Symmetric Generation
      {
	SecretKey	key;
	Time		time;

	time.Start();

	// generate the new keys.
	if (key.Generate(Benchmark::SymmetricKeyLength) == StatusError)
	  escape("unable to generate a new secret key");

	time.Stop();

	std::cout << "Symmetric[" << Benchmark::SymmetricKeyLength
		  << "] Generation: ";
	time.Print();
	std::cout << std::endl;
      }

      // Symmetric Encryption
      {
	SecretKey	key;
	Region		region;
	Cipher		cipher;
	Time		time;

	// generate the new keys.
	if (key.Generate(Benchmark::SymmetricKeyLength) == StatusError)
	  escape("unable to generate a new secret key");

	// prepare random data.
	if (region.Wrap(buffer, sizeof (buffer)) == StatusError)
	  escape("unable to prepare the block");

	time.Start();

	// encrypt the input.
	if (key.Encrypt(region, cipher) == StatusError)
	  escape("unable to encrypt the block");

	time.Stop();

	std::cout << "Symmetric[" << Benchmark::SymmetricKeyLength
		  << "] Block[" << Benchmark::BlockSize << "] Encryption: ";
	time.Print();
	std::cout << std::endl;
      }

      // Symmetric Decryption
      {
	SecretKey	key;
	Region		region;
	Cipher		cipher;
	Clear		clear;
	Time		time;

	// generate the new keys.
	if (key.Generate(Benchmark::SymmetricKeyLength) == StatusError)
	  escape("unable to generate a new secret key");

	// prepare random data.
	if (region.Wrap(buffer, sizeof (buffer)) == StatusError)
	  escape("unable to prepare the block");

	// encrypt the input.
	if (key.Encrypt(region, cipher) == StatusError)
	  escape("unable to encrypt the block");

	time.Start();

	// decrypt the input.
	if (key.Decrypt(cipher, clear) == StatusError)
	  escape("unable to decrypt the block");

	time.Stop();

	std::cout << "Symmetric[" << Benchmark::SymmetricKeyLength
		  << "] Block[" << Benchmark::BlockSize << "] Decryption: ";
	time.Print();
	std::cout << std::endl;
      }

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
  if (elle::benchmark::Main() == elle::radix::StatusError)
    {
      show();

      return (1);
    }

  return (0);
}
