//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/crypto/PublicKey.hxx
//
// created       julien quintard   [mon jan 26 14:09:50 2009]
// updated       julien quintard   [fri jul 31 01:07:30 2009]
//

#ifndef ELLE_CRYPTO_PUBLICKEY_HXX
#define ELLE_CRYPTO_PUBLICKEY_HXX

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
    /// note that the code is replicated in order to provide optimisation.
    /// Indeed, otherwise, everytime a single item is hashed, the whole 9-step
    /// ifs would be executed, testing if there are more than one item
    /// to hash.
    ///

    ///
    /// encrypt
    ///

    ///
    /// this method takes any object that implements the Archivable
    /// interface and encrypts it.
    ///
    /// the process obviously consists in serializing the object first.
    ///
    /// note that Encrypt() methods return an archive hidden in the
    /// code.
    ///
    template <typename T1>
    Status		PublicKey::Encrypt(const T1&		o1,
					   Code&		code) const
    {
      Archive		archive;

      printf("[XXX] %s\n", __PRETTY_FUNCTION__);

      // create an archive.
      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      // serialize the object.
      if (archive.Serialize(o1) == StatusError)
	escape("unable to serialize the object");

      // encrypt the archive.
      if (this->Encrypt(archive, code) == StatusError)
	escape("unable to encrypt the object's archive");

      leave();
    }

    template <typename T1,
	      typename T2>
    Status		PublicKey::Encrypt(const T1&		o1,
					   const T2&		o2,
					   Code&		code) const
    {
      Archive		archive;

      printf("[XXX] %s\n", __PRETTY_FUNCTION__);

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2) == StatusError)
	escape("unable to serialize the object");

      if (this->Encrypt(archive, code) == StatusError)
	escape("unable to encrypt the object's archive");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3>
    Status		PublicKey::Encrypt(const T1&		o1,
					   const T2&		o2,
					   const T3&		o3,
					   Code&		code) const
    {
      Archive		archive;

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3) == StatusError)
	escape("unable to serialize the object");

      if (this->Encrypt(archive, code) == StatusError)
	escape("unable to encrypt the object's archive");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4>
    Status		PublicKey::Encrypt(const T1&		o1,
					   const T2&		o2,
					   const T3&		o3,
					   const T4&		o4,
					   Code&		code) const
    {
      Archive		archive;

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4) == StatusError)
	escape("unable to serialize the object");

      if (this->Encrypt(archive, code) == StatusError)
	escape("unable to encrypt the object's archive");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5>
    Status		PublicKey::Encrypt(const T1&		o1,
					   const T2&		o2,
					   const T3&		o3,
					   const T4&		o4,
					   const T5&		o5,
					   Code&		code) const
    {
      Archive		archive;

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5) == StatusError)
	escape("unable to serialize the object");

      if (this->Encrypt(archive, code) == StatusError)
	escape("unable to encrypt the object's archive");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6>
    Status		PublicKey::Encrypt(const T1&		o1,
					   const T2&		o2,
					   const T3&		o3,
					   const T4&		o4,
					   const T5&		o5,
					   const T6&		o6,
					   Code&		code) const
    {
      Archive		archive;

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5, o6) == StatusError)
	escape("unable to serialize the object");

      if (this->Encrypt(archive, code) == StatusError)
	escape("unable to encrypt the object's archive");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7>
    Status		PublicKey::Encrypt(const T1&		o1,
					   const T2&		o2,
					   const T3&		o3,
					   const T4&		o4,
					   const T5&		o5,
					   const T6&		o6,
					   const T7&		o7,
					   Code&		code) const
    {
      Archive		archive;

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5, o6, o7) == StatusError)
	escape("unable to serialize the object");

      if (this->Encrypt(archive, code) == StatusError)
	escape("unable to encrypt the object's archive");

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
    Status		PublicKey::Encrypt(const T1&		o1,
					   const T2&		o2,
					   const T3&		o3,
					   const T4&		o4,
					   const T5&		o5,
					   const T6&		o6,
					   const T7&		o7,
					   const T8&		o8,
					   Code&		code) const
    {
      Archive		archive;

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5, o6, o7, o8) == StatusError)
	escape("unable to serialize the object");

      if (this->Encrypt(archive, code) == StatusError)
	escape("unable to encrypt the object's archive");

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
    Status		PublicKey::Encrypt(const T1&		o1,
					   const T2&		o2,
					   const T3&		o3,
					   const T4&		o4,
					   const T5&		o5,
					   const T6&		o6,
					   const T7&		o7,
					   const T8&		o8,
					   const T9&		o9,
					   Code&		code) const
    {
      Archive		archive;

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5, o6, o7, o8, o9) == StatusError)
	escape("unable to serialize the object");

      if (this->Encrypt(archive, code) == StatusError)
	escape("unable to encrypt the object's archive");

      leave();
    }

    ///
    /// verify
    ///

    ///
    /// this method verifies an Archivable object by serializing it
    /// before performing the verification process.
    ///
    template <typename T1>
    Status		PublicKey::Verify(const Signature&	signature,
					  const T1&		o1) const
    {
      Archive		archive;

      printf("[XXX] %s\n", __PRETTY_FUNCTION__);

      // create the archive.
      if (archive.Create() == StatusError)
	flee("unable to create the archive");

      // serialize the object.
      if (archive.Serialize(o1) == StatusError)
	flee("unable to serialize the object");

      // call the Verify() method.
      if (this->Verify(signature, archive) != StatusTrue)
	flee("unable to verify the signature against the object's archive");

      true();
    }

    template <typename T1,
	      typename T2>
    Status		PublicKey::Verify(const Signature&	signature,
					  const T1&		o1,
					  const T2&		o2) const
    {
      Archive		archive;

      printf("[XXX] %s\n", __PRETTY_FUNCTION__);

      if (archive.Create() == StatusError)
	flee("unable to create the archive");

      if (archive.Serialize(o1, o2) == StatusError)
	flee("unable to serialize the objects");

      if (this->Verify(signature, archive) != StatusTrue)
	flee("unable to verify the signature against the objects' archive");

      true();
    }

    template <typename T1,
	      typename T2,
	      typename T3>
    Status		PublicKey::Verify(const Signature&	signature,
					  const T1&		o1,
					  const T2&		o2,
					  const T3&		o3) const
    {
      Archive		archive;

      if (archive.Create() == StatusError)
	flee("unable to create the archive");

      if (archive.Serialize(o1, o2, o3) == StatusError)
	flee("unable to serialize the objects");

      if (this->Verify(signature, archive) != StatusTrue)
	flee("unable to verify the signature against the objects' archive");

      true();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4>
    Status		PublicKey::Verify(const Signature&	signature,
					  const T1&		o1,
					  const T2&		o2,
					  const T3&		o3,
					  const T4&		o4) const
    {
      Archive		archive;

      if (archive.Create() == StatusError)
	flee("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4) == StatusError)
	flee("unable to serialize the objects");

      if (this->Verify(signature, archive) != StatusTrue)
	flee("unable to verify the signature against the objects' archive");

      true();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5>
    Status		PublicKey::Verify(const Signature&	signature,
					  const T1&		o1,
					  const T2&		o2,
					  const T3&		o3,
					  const T4&		o4,
					  const T5&		o5) const
    {
      Archive		archive;

      if (archive.Create() == StatusError)
	flee("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5) == StatusError)
	flee("unable to serialize the objects");

      if (this->Verify(signature, archive) != StatusTrue)
	flee("unable to verify the signature against the objects' archive");

      true();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6>
    Status		PublicKey::Verify(const Signature&	signature,
					  const T1&		o1,
					  const T2&		o2,
					  const T3&		o3,
					  const T4&		o4,
					  const T5&		o5,
					  const T6&		o6) const
    {
      Archive		archive;

      if (archive.Create() == StatusError)
	flee("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5, o6) == StatusError)
	flee("unable to serialize the objects");

      if (this->Verify(signature, archive) != StatusTrue)
	flee("unable to verify the signature against the objects' archive");

      true();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7>
    Status		PublicKey::Verify(const Signature&	signature,
					  const T1&		o1,
					  const T2&		o2,
					  const T3&		o3,
					  const T4&		o4,
					  const T5&		o5,
					  const T6&		o6,
					  const T7&		o7) const
    {
      Archive		archive;

      if (archive.Create() == StatusError)
	flee("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5, o6, o7) == StatusError)
	flee("unable to serialize the objects");

      if (this->Verify(signature, archive) != StatusTrue)
	flee("unable to verify the signature against the objects' archive");

      true();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7,
	      typename T8>
    Status		PublicKey::Verify(const Signature&	signature,
					  const T1&		o1,
					  const T2&		o2,
					  const T3&		o3,
					  const T4&		o4,
					  const T5&		o5,
					  const T6&		o6,
					  const T7&		o7,
					  const T8&		o8) const
    {
      Archive		archive;

      if (archive.Create() == StatusError)
	flee("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5, o6, o7, o8) == StatusError)
	flee("unable to serialize the objects");

      if (this->Verify(signature, archive) != StatusTrue)
	flee("unable to verify the signature against the objects' archive");

      true();
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
    Status		PublicKey::Verify(const Signature&	signature,
					  const T1&		o1,
					  const T2&		o2,
					  const T3&		o3,
					  const T4&		o4,
					  const T5&		o5,
					  const T6&		o6,
					  const T7&		o7,
					  const T8&		o8,
					  const T9&		o9) const
    {
      Archive		archive;

      if (archive.Create() == StatusError)
	flee("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5, o6, o7, o8, o9) == StatusError)
	flee("unable to serialize the objects");

      if (this->Verify(signature, archive) != StatusTrue)
	flee("unable to verify the signature against the objects' archive");

      true();
    }

  }
}

#endif
