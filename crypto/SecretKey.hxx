//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/crypto/SecretKey.hxx
//
// created       julien quintard   [mon jan 26 14:09:50 2009]
// updated       julien quintard   [mon mar  1 13:05:50 2010]
//

#ifndef ELLE_CRYPTO_SECRETKEY_HXX
#define ELLE_CRYPTO_SECRETKEY_HXX

namespace elle
{
  using namespace core;
  using namespace misc;
  using namespace archive;

  namespace crypto
  {

//
// ---------- variadic templates ----------------------------------------------
//

    ///
    /// these methods make it easier to decrypt/sign multiple items at
    /// the same time while keeping a way to catch errors.
    ///
    /// note that the Encrypt() methods cannot be factorised through
    /// variadic templates since the last argument is not a variadic
    /// argument.
    ///

    //
    // encrypt
    //

    ///
    /// this method encrypts the given object by serializing it.
    ///
    template <typename T1>
    Status		SecretKey::Encrypt(const T1&		o1,
					   Cipher&		cipher) const
    {
      Archive		archive;

      enter();

      // create an archive.
      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      // serialize the object.
      if (archive.Serialize(o1) == StatusError)
	escape("unable to serialize the object");

      // encrypt the archive.
      if (this->Encrypt(archive, cipher) == StatusError)
	escape("unable to encrypt the object's archive");

      leave();
    }

    template <typename T1,
	      typename T2> 
   Status		SecretKey::Encrypt(const T1&		o1,
					   const T2&		o2,
					   Cipher&		cipher) const
    {
      Archive		archive;

      enter();

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2) == StatusError)
	escape("unable to serialize the objects");

      if (this->Encrypt(archive, cipher) == StatusError)
	escape("unable to encrypt the objects' archive");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3>
    Status		SecretKey::Encrypt(const T1&		o1,
					   const T2&		o2,
					   const T3&		o3,
					   Cipher&		cipher) const
    {
      Archive		archive;

      enter();

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3) == StatusError)
	escape("unable to serialize the objects");

      if (this->Encrypt(archive, cipher) == StatusError)
	escape("unable to encrypt the objects' archive");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4>
    Status		SecretKey::Encrypt(const T1&		o1,
					   const T2&		o2,
					   const T3&		o3,
					   const T4&		o4,
					   Cipher&		cipher) const
    {
      Archive		archive;

      enter();

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4) == StatusError)
	escape("unable to serialize the objects");

      if (this->Encrypt(archive, cipher) == StatusError)
	escape("unable to encrypt the objects' archive");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5>
    Status		SecretKey::Encrypt(const T1&		o1,
					   const T2&		o2,
					   const T3&		o3,
					   const T4&		o4,
					   const T5&		o5,
					   Cipher&		cipher) const
    {
      Archive		archive;

      enter();

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5) == StatusError)
	escape("unable to serialize the objects");

      if (this->Encrypt(archive, cipher) == StatusError)
	escape("unable to encrypt the objects' archive");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6>
    Status		SecretKey::Encrypt(const T1&		o1,
					   const T2&		o2,
					   const T3&		o3,
					   const T4&		o4,
					   const T5&		o5,
					   const T6&		o6,
					   Cipher&		cipher) const
    {
      Archive		archive;

      enter();

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5, o6) == StatusError)
	escape("unable to serialize the objects");

      if (this->Encrypt(archive, cipher) == StatusError)
	escape("unable to encrypt the objects' archive");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7>
    Status		SecretKey::Encrypt(const T1&		o1,
					   const T2&		o2,
					   const T3&		o3,
					   const T4&		o4,
					   const T5&		o5,
					   const T6&		o6,
					   const T7&		o7,
					   Cipher&		cipher) const
    {
      Archive		archive;

      enter();

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5, o6, o7) == StatusError)
	escape("unable to serialize the objects");

      if (this->Encrypt(archive, cipher) == StatusError)
	escape("unable to encrypt the objects' archive");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7,
	      typename T8>
    Status		SecretKey::Encrypt(const T1&		o1,
					   const T2&		o2,
					   const T3&		o3,
					   const T4&		o4,
					   const T5&		o5,
					   const T6&		o6,
					   const T7&		o7,
					   const T8&		o8,
					   Cipher&		cipher) const
    {
      Archive		archive;

      enter();

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5, o6, o7, o8) == StatusError)
	escape("unable to serialize the objects");

      if (this->Encrypt(archive, cipher) == StatusError)
	escape("unable to encrypt the objects' archive");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7,
	      typename T8,
	      typename T9>
    Status		SecretKey::Encrypt(const T1&		o1,
					   const T2&		o2,
					   const T3&		o3,
					   const T4&		o4,
					   const T5&		o5,
					   const T6&		o6,
					   const T7&		o7,
					   const T8&		o8,
					   const T9&		o9,
					   Cipher&		cipher) const
    {
      Archive		archive;

      enter();

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5, o6, o7, o8, o9) == StatusError)
	escape("unable to serialize the objects");

      if (this->Encrypt(archive, cipher) == StatusError)
	escape("unable to encrypt the objects' archive");

      leave();
    }

    //
    // decrypt
    //

    ///
    /// this method decrypts the given Cipher before extracting the
    /// object from the Clear.
    ///
    template <typename T>
    Status		SecretKey::Decrypt(const Cipher&	cipher,
					   T&			parameter)
      const
    {
      Archive		archive;
      Clear		clear;

      enter();

      // decrypt the cipher.
      if (this->Decrypt(cipher, clear) == StatusError)
	escape("unable to decrypt the cipher");

      // wrap the clear into an archive.
      if (archive.Prepare(clear) == StatusError)
	escape("unable to prepare the archive");

      // detach the data so that not both the clear and archive
      // release the data.
      if (archive.Detach() == StatusError)
	escape("unable to detach the archive's data");

      // extract the object.
      if (archive.Extract(parameter) == StatusError)
	escape("unable to extract the object");

      leave();
    }

    ///
    /// this method decrypts a set of items.
    ///
    template <typename T,
	      typename... TT>
    Status		SecretKey::Decrypt(const Cipher&	cipher,
					   T&			parameter,
					   TT&...		parameters)
      const
    {
      Archive		archive;
      Clear		clear;

      enter();

      if (this->Decrypt(cipher, clear) == StatusError)
	escape("unable to decrypt the cipher");

      if (archive.Prepare(clear) == StatusError)
	escape("unable to prepare the archive");

      if (archive.Detach() == StatusError)
	escape("unable to detach the archive's data");

      if (archive.Extract(parameter, parameters...) == StatusError)
	escape("unable to extract the objects");

      leave();
    }

  }
}

#endif
