//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/user/Agent.hxx
//
// created       julien quintard   [mon jan 26 14:09:50 2009]
// updated       julien quintard   [thu apr 22 14:53:02 2010]
//

#ifndef ETOILE_USER_AGENT_HXX
#define ETOILE_USER_AGENT_HXX

namespace etoile
{
  namespace user
  {

//
// ---------- variadic templates ----------------------------------------------
//

    ///
    /// these methods make it easier to encrypt/decrypt/sign/verify
    /// multiple items at the same time while keeping a way to catch errors.
    ///

    //
    // encrypt
    //

    ///
    /// this method takes any object and encrypts it by forwarding the
    /// call the the public key.
    ///
    template <typename... T>
    Status		Agent::Encrypt(T&...			parameters)
      const
    {
      return (this->K.Encrypt(parameters...));
    }

    //
    // decrypt
    //

    ///
    /// this method decrypts a code and returns a pretty newly created
    /// object.
    ///
    template <typename T,
	      typename... TT>
    Status		Agent::Decrypt(const Code&		code,
				       T&			parameter,
				       TT&...			parameters)
      const
    {
      Archive		archive;
      Clear		clear;

      enter();

      // decrypt the code.
      if (this->Decrypt(code, clear) == StatusError)
	escape("unable to decrypt the code");

      // detach the data so that not both the clear and archive
      // release the data.
      if (clear.Detach() == StatusError)
	escape("unable to detach the clear's data");

      // wrap the clear into an archive.
      if (archive.Prepare(clear) == StatusError)
	escape("unable to prepare the archive");

      // extract the item.
      if (archive.Extract(parameter, parameters...) == StatusError)
	escape("unable to extract the items");

      leave();
    }

    //
    // sign
    //

    ///
    /// this method signs an object by serializing it before applying the
    /// signature process.
    ///
    template <typename T1>
    Status		Agent::Sign(const T1&			o1,
				    Signature&			signature)
      const
    {
      Archive		archive;
      Digest		digest;

      enter();

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
    Status		Agent::Sign(const T1&			o1,
				    const T2&			o2,
				    Signature&			signature)
      const
    {
      Archive		archive;
      Digest		digest;

      enter();

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
    Status		Agent::Sign(const T1&			o1,
				    const T2&			o2,
				    const T3&			o3,
				    Signature&			signature)
      const
    {
      Archive		archive;
      Digest		digest;

      enter();

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
    Status		Agent::Sign(const T1&			o1,
				    const T2&			o2,
				    const T3&			o3,
				    const T4&			o4,
				    Signature&			signature)
      const
    {
      Archive		archive;
      Digest		digest;

      enter();

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
    Status		Agent::Sign(const T1&			o1,
				    const T2&			o2,
				    const T3&			o3,
				    const T4&			o4,
				    const T5&			o5,
				    Signature&			signature)
      const
    {
      Archive		archive;
      Digest		digest;

      enter();

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
    Status		Agent::Sign(const T1&			o1,
				    const T2&			o2,
				    const T3&			o3,
				    const T4&			o4,
				    const T5&			o5,
				    const T6&			o6,
				    Signature&			signature)
      const
    {
      Archive		archive;
      Digest		digest;

      enter();

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
    Status		Agent::Sign(const T1&			o1,
				    const T2&			o2,
				    const T3&			o3,
				    const T4&			o4,
				    const T5&			o5,
				    const T6&			o6,
				    const T7&			o7,
				    Signature&			signature)
      const
    {
      Archive		archive;
      Digest		digest;

      enter();

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
    Status		Agent::Sign(const T1&			o1,
				    const T2&			o2,
				    const T3&			o3,
				    const T4&			o4,
				    const T5&			o5,
				    const T6&			o6,
				    const T7&			o7,
				    const T8&			o8,
				    Signature&			signature)
      const
    {
      Archive		archive;
      Digest		digest;

      enter();

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
    Status		Agent::Sign(const T1&			o1,
				    const T2&			o2,
				    const T3&			o3,
				    const T4&			o4,
				    const T5&			o5,
				    const T6&			o6,
				    const T7&			o7,
				    const T8&			o8,
				    const T9&			o9,
				    Signature&			signature)
      const
    {
      Archive		archive;
      Digest		digest;

      enter();

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5, o6, o7, o8, o9) == StatusError)
	escape("unable to serialize the objects");

      if (this->Sign(archive, signature) == StatusError)
	escape("unable to sign the objects' archive");

      leave();
    }

    //
    // verify
    //

    ///
    /// this method verifies a signature by calling the Verify() method
    /// of the public key.
    ///
    template <typename... T>
    Status		Agent::Verify(T&...			parameters)
      const
    {
      return (this->K.Verify(parameters...));
    }

  }
}

#endif
