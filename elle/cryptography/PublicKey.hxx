//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/cryptography/PublicKey.hxx
//
// created       julien quintard   [mon jan 26 14:09:50 2009]
// updated       julien quintard   [thu mar 11 16:46:48 2010]
//

#ifndef ELLE_CRYPTOGRAPHY_PUBLICKEY_HXX
#define ELLE_CRYPTOGRAPHY_PUBLICKEY_HXX

namespace elle
{
  using namespace core;
  using namespace misc;
  using namespace archive;

  namespace cryptography
  {

//
// ---------- variadic templates ----------------------------------------------
//

    ///
    /// these methods make it easier to decrypt/sign multiple items at
    /// the same time while keeping a way to catch errors.
    ///
    /// note that the Encrypt() method cannot benefit from the variadic
    /// template arguments.
    ///

    //
    // encrypt
    //

    ///
    /// this method takes any object and encrypts it.
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

      enter();

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

      enter();

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

      enter();

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

      enter();

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

      enter();

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

      enter();

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

      enter();

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

      enter();

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

      enter();

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5, o6, o7, o8, o9) == StatusError)
	escape("unable to serialize the object");

      if (this->Encrypt(archive, code) == StatusError)
	escape("unable to encrypt the object's archive");

      leave();
    }

    //
    // verify
    //

    ///
    /// this method verifies an object by serializing it before performing
    /// the verification process.
    ///
    template <typename T>
    Status		PublicKey::Verify(const Signature&	signature,
					  const T&		parameter)
      const
    {
      Archive		archive;

      enter();

      // create the archive.
      if (archive.Create() == StatusError)
	flee("unable to create the archive");

      // serialize the object.
      if (archive.Serialize(parameter) == StatusError)
	flee("unable to serialize the object");

      // call the Verify() method.
      if (this->Verify(signature, archive) != StatusTrue)
	flee("unable to verify the signature against the object's archive");

      true();
    }

    ///
    /// this method verifies a set of items.
    ///
    template <typename T,
	      typename... TT>
    Status		PublicKey::Verify(const Signature&	signature,
					  const T&		parameter,
					  const TT&...		parameters)
      const
    {
      Archive		archive;

      enter();

      if (archive.Create() == StatusError)
	flee("unable to create the archive");

      if (archive.Serialize(parameter, parameters...) == StatusError)
	flee("unable to serialize the objects");

      if (this->Verify(signature, archive) != StatusTrue)
	flee("unable to verify the signature against the objects' archive");

      true();
    }

  }
}

#endif
