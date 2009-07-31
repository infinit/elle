//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/crypto/PrivateKey.hxx
//
// created       julien quintard   [mon jan 26 14:09:50 2009]
// updated       julien quintard   [fri jul 31 01:20:44 2009]
//

#ifndef ELLE_CRYPTO_PRIVATEKEY_HXX
#define ELLE_CRYPTO_PRIVATEKEY_HXX

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

    //
    // decrypt
    //

    ///
    /// this method decrypts a code and returns a pretty newly created
    /// archivable object.
    ///
    template <typename T1>
    Status		PrivateKey::Decrypt(const Code&		code,
					    T1&			o1)
    {
      Archive		archive;
      Clear		clear;

      printf("[XXX] %s\n", __PRETTY_FUNCTION__);
      sleep(3);

      // decrypt the code.
      if (this->Decrypt(code, clear) == StatusError)
	escape("unable to decrypt the code");

      // wrap the clear into an archive.
      if (archive.Prepare(clear) == StatusError)
	escape("unable to prepare the archive");

      // detach the data so that not both the clear and archive
      // release the data.
      if (archive.Detach() == StatusError)
	escape("unable to detach the archive's data");

      // extract the item.
      if (archive.Extract(o1) == StatusError)
	escape("unable to extract the item");

      leave();
    }

    template <typename T1,
	      typename T2>
    Status		PrivateKey::Decrypt(const Code&		code,
					    T1&			o1,
					    T2&			o2)
    {
      Archive		archive;
      Clear		clear;

      printf("[XXX] %s\n", __PRETTY_FUNCTION__);

      if (this->Decrypt(code, clear) == StatusError)
	escape("unable to decrypt the code");

      if (archive.Prepare(clear) == StatusError)
	escape("unable to prepare the archive");

      if (archive.Detach() == StatusError)
	escape("unable to detach the archive's data");

      if (archive.Extract(o1, o2) == StatusError)
	escape("unable to extract the items");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3>
    Status		PrivateKey::Decrypt(const Code&		code,
					    T1&			o1,
					    T2&			o2,
					    T3&			o3)
    {
      Archive		archive;
      Clear		clear;

      if (this->Decrypt(code, clear) == StatusError)
	escape("unable to decrypt the code");

      if (archive.Prepare(clear) == StatusError)
	escape("unable to prepare the archive");

      if (archive.Detach() == StatusError)
	escape("unable to detach the archive's data");

      if (archive.Extract(o1, o2, o3) == StatusError)
	escape("unable to extract the items");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4>
    Status		PrivateKey::Decrypt(const Code&		code,
					    T1&			o1,
					    T2&			o2,
					    T3&			o3,
					    T4&			o4)
    {
      Archive		archive;
      Clear		clear;

      if (this->Decrypt(code, clear) == StatusError)
	escape("unable to decrypt the code");

      if (archive.Prepare(clear) == StatusError)
	escape("unable to prepare the archive");

      if (archive.Detach() == StatusError)
	escape("unable to detach the archive's data");

      if (archive.Extract(o1, o2, o3, o4) == StatusError)
	escape("unable to extract the items");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5>
    Status		PrivateKey::Decrypt(const Code&		code,
					    T1&			o1,
					    T2&			o2,
					    T3&			o3,
					    T4&			o4,
					    T5&			o5)
    {
      Archive		archive;
      Clear		clear;

      if (this->Decrypt(code, clear) == StatusError)
	escape("unable to decrypt the code");

      if (archive.Prepare(clear) == StatusError)
	escape("unable to prepare the archive");

      if (archive.Detach() == StatusError)
	escape("unable to detach the archive's data");

      if (archive.Extract(o1, o2, o3, o4, o5) == StatusError)
	escape("unable to extract the items");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6>
    Status		PrivateKey::Decrypt(const Code&		code,
					    T1&			o1,
					    T2&			o2,
					    T3&			o3,
					    T4&			o4,
					    T5&			o5,
					    T6&			o6)
    {
      Archive		archive;
      Clear		clear;

      if (this->Decrypt(code, clear) == StatusError)
	escape("unable to decrypt the code");

      if (archive.Prepare(clear) == StatusError)
	escape("unable to prepare the archive");

      if (archive.Detach() == StatusError)
	escape("unable to detach the archive's data");

      if (archive.Extract(o1, o2, o3, o4, o5, o6) == StatusError)
	escape("unable to extract the items");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7>
    Status		PrivateKey::Decrypt(const Code&		code,
					    T1&			o1,
					    T2&			o2,
					    T3&			o3,
					    T4&			o4,
					    T5&			o5,
					    T6&			o6,
					    T7&			o7)
    {
      Archive		archive;
      Clear		clear;

      if (this->Decrypt(code, clear) == StatusError)
	escape("unable to decrypt the code");

      if (archive.Prepare(clear) == StatusError)
	escape("unable to prepare the archive");

      if (archive.Detach() == StatusError)
	escape("unable to detach the archive's data");

      if (archive.Extract(o1, o2, o3, o4, o5, o6, o7) == StatusError)
	escape("unable to extract the items");

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
    Status		PrivateKey::Decrypt(const Code&		code,
					    T1&			o1,
					    T2&			o2,
					    T3&			o3,
					    T4&			o4,
					    T5&			o5,
					    T6&			o6,
					    T7&			o7,
					    T8&			o8)
    {
      Archive		archive;
      Clear		clear;

      if (this->Decrypt(code, clear) == StatusError)
	escape("unable to decrypt the code");

      if (archive.Prepare(clear) == StatusError)
	escape("unable to prepare the archive");

      if (archive.Detach() == StatusError)
	escape("unable to detach the archive's data");

      if (archive.Extract(o1, o2, o3, o4, o5, o6, o7, o8) == StatusError)
	escape("unable to extract the items");

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
    Status		PrivateKey::Decrypt(const Code&		code,
					    T1&			o1,
					    T2&			o2,
					    T3&			o3,
					    T4&			o4,
					    T5&			o5,
					    T6&			o6,
					    T7&			o7,
					    T8&			o8,
					    T9&			o9)
    {
      Archive		archive;
      Clear		clear;

      if (this->Decrypt(code, clear) == StatusError)
	escape("unable to decrypt the code");

      if (archive.Prepare(clear) == StatusError)
	escape("unable to prepare the archive");

      if (archive.Detach() == StatusError)
	escape("unable to detach the archive's data");

      if (archive.Extract(o1, o2, o3, o4, o5, o6, o7, o8, o9) == StatusError)
	escape("unable to extract the items");

      leave();
    }

    //
    // sign
    //

    ///
    /// this method signs an Archivable object by serializing it
    /// before applying the signature process.
    ///
    template <typename T1>
    Status		PrivateKey::Sign(const T1&		o1,
					 Signature&		signature)
      const
    {
      Archive		archive;
      Digest		digest;

      printf("[XXX] %s\n", __PRETTY_FUNCTION__);

      // create th archive.
      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      // serialize the object.
      if (archive.Serialize(o1) == StatusError)
	escape("unable to serialize the object");

      // re-launch the Sign() method for a digest.
      if (this->Sign(archive, signature) == StatusError)
	escape("unable to sign the object's archive");

      leave();
    }

    template <typename T1,
	      typename T2>
    Status		PrivateKey::Sign(const T1&		o1,
					 const T2&		o2,
					 Signature&		signature)
      const
    {
      Archive		archive;
      Digest		digest;

      printf("[XXX] %s\n", __PRETTY_FUNCTION__);

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2) == StatusError)
	escape("unable to serialize the objects");

      if (this->Sign(archive, signature) == StatusError)
	escape("unable to sign the objects' archive");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3>
    Status		PrivateKey::Sign(const T1&		o1,
					 const T2&		o2,
					 const T3&		o3,
					 Signature&		signature)
      const
    {
      Archive		archive;
      Digest		digest;

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3) == StatusError)
	escape("unable to serialize the objects");

      if (this->Sign(archive, signature) == StatusError)
	escape("unable to sign the objects' archive");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4>
    Status		PrivateKey::Sign(const T1&		o1,
					 const T2&		o2,
					 const T3&		o3,
					 const T4&		o4,
					 Signature&		signature)
      const
    {
      Archive		archive;
      Digest		digest;

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4) == StatusError)
	escape("unable to serialize the objects");

      if (this->Sign(archive, signature) == StatusError)
	escape("unable to sign the objects' archive");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5>
    Status		PrivateKey::Sign(const T1&		o1,
					 const T2&		o2,
					 const T3&		o3,
					 const T4&		o4,
					 const T5&		o5,
					 Signature&		signature)
      const
    {
      Archive		archive;
      Digest		digest;

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5) == StatusError)
	escape("unable to serialize the objects");

      if (this->Sign(archive, signature) == StatusError)
	escape("unable to sign the objects' archive");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6>
    Status		PrivateKey::Sign(const T1&		o1,
					 const T2&		o2,
					 const T3&		o3,
					 const T4&		o4,
					 const T5&		o5,
					 const T6&		o6,
					 Signature&		signature)
      const
    {
      Archive		archive;
      Digest		digest;

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5, o6) == StatusError)
	escape("unable to serialize the objects");

      if (this->Sign(archive, signature) == StatusError)
	escape("unable to sign the objects' archive");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7>
    Status		PrivateKey::Sign(const T1&		o1,
					 const T2&		o2,
					 const T3&		o3,
					 const T4&		o4,
					 const T5&		o5,
					 const T6&		o6,
					 const T7&		o7,
					 Signature&		signature)
      const
    {
      Archive		archive;
      Digest		digest;

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5, o6, o7) == StatusError)
	escape("unable to serialize the objects");

      if (this->Sign(archive, signature) == StatusError)
	escape("unable to sign the objects' archive");

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
    Status		PrivateKey::Sign(const T1&		o1,
					 const T2&		o2,
					 const T3&		o3,
					 const T4&		o4,
					 const T5&		o5,
					 const T6&		o6,
					 const T7&		o7,
					 const T8&		o8,
					 Signature&		signature)
      const
    {
      Archive		archive;
      Digest		digest;

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5, o6, o7, o8) == StatusError)
	escape("unable to serialize the objects");

      if (this->Sign(archive, signature) == StatusError)
	escape("unable to sign the objects' archive");

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
    Status		PrivateKey::Sign(const T1&		o1,
					 const T2&		o2,
					 const T3&		o3,
					 const T4&		o4,
					 const T5&		o5,
					 const T6&		o6,
					 const T7&		o7,
					 const T8&		o8,
					 const T9&		o9,
					 Signature&		signature)
      const
    {
      Archive		archive;
      Digest		digest;

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5, o6, o7, o8, o9) == StatusError)
	escape("unable to serialize the objects");

      if (this->Sign(archive, signature) == StatusError)
	escape("unable to sign the objects' archive");

      leave();
    }

  }
}

#endif
