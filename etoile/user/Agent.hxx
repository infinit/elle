//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/user/Agent.hxx
//
// created       julien quintard   [sat may  7 22:29:35 2011]
// updated       julien quintard   [sat may  7 22:36:33 2011]
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
    /// these methods make it easier to decrypt/sign multiple items at
    /// the same time while keeping a way to catch errors.
    ///
    /// note that the Sign() methods cannot benefit from variadic templates
    /// since the last argument is not the variadic argument.
    ///

    //
    // decrypt
    //

    ///
    /// this method decrypts a code and returns a pretty newly created
    /// object.
    ///
    template <typename T,
	      typename... TT>
    elle::Status	Agent::Decrypt(const elle::Code&	code,
				       T&			parameter,
				       TT&...			parameters)
      const
    {
      elle::Archive	archive;
      elle::Clear	clear;

      enter();

      // decrypt the code.
      if (this->Decrypt(code, clear) == elle::StatusError)
	escape("unable to decrypt the code");

      // wrap the clear into an archive.
      if (archive.Prepare(clear) == elle::StatusError)
	escape("unable to prepare the archive");

      // detach the data so that not both the clear and archive
      // release the data.
      if (clear.Detach() == elle::StatusError)
	escape("unable to detach the clear's data");

      // extract the item.
      if (archive.Extract(parameter, parameters...) == elle::StatusError)
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
    elle::Status	Agent::Sign(const T1&			o1,
				    elle::Signature&		signature)
      const
    {
      elle::Archive	archive;
      elle::Digest	digest;

      enter();

      // create th archive.
      if (archive.Create() == elle::StatusError)
	escape("unable to create the archive");

      // serialize the object.
      if (archive.Serialize(o1) == elle::StatusError)
	escape("unable to serialize the object");

      // re-launch the Sign() method for a digest.
      if (this->Sign(archive, signature) == elle::StatusError)
	escape("unable to sign the object's archive");

      leave();
    }

    template <typename T1,
	      typename T2>
    elle::Status	Agent::Sign(const T1&			o1,
				    const T2&			o2,
				    elle::Signature&		signature)
      const
    {
      elle::Archive	archive;
      elle::Digest	digest;

      enter();

      if (archive.Create() == elle::StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2) == elle::StatusError)
	escape("unable to serialize the objects");

      if (this->Sign(archive, signature) == elle::StatusError)
	escape("unable to sign the objects' archive");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3>
    elle::Status	Agent::Sign(const T1&			o1,
				    const T2&			o2,
				    const T3&			o3,
				    elle::Signature&		signature)
      const
    {
      elle::Archive	archive;
      elle::Digest	digest;

      enter();

      if (archive.Create() == elle::StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3) == elle::StatusError)
	escape("unable to serialize the objects");

      if (this->Sign(archive, signature) == elle::StatusError)
	escape("unable to sign the objects' archive");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4>
    elle::Status	Agent::Sign(const T1&			o1,
				    const T2&			o2,
				    const T3&			o3,
				    const T4&			o4,
				    elle::Signature&		signature)
      const
    {
      elle::Archive	archive;
      elle::Digest	digest;

      enter();

      if (archive.Create() == elle::StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4) == elle::StatusError)
	escape("unable to serialize the objects");

      if (this->Sign(archive, signature) == elle::StatusError)
	escape("unable to sign the objects' archive");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5>
    elle::Status	Agent::Sign(const T1&			o1,
				    const T2&			o2,
				    const T3&			o3,
				    const T4&			o4,
				    const T5&			o5,
				    elle::Signature&		signature)
      const
    {
      elle::Archive	archive;
      elle::Digest	digest;

      enter();

      if (archive.Create() == elle::StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5) == elle::StatusError)
	escape("unable to serialize the objects");

      if (this->Sign(archive, signature) == elle::StatusError)
	escape("unable to sign the objects' archive");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6>
    elle::Status	Agent::Sign(const T1&			o1,
				    const T2&			o2,
				    const T3&			o3,
				    const T4&			o4,
				    const T5&			o5,
				    const T6&			o6,
				    elle::Signature&		signature)
      const
    {
      elle::Archive	archive;
      elle::Digest	digest;

      enter();

      if (archive.Create() == elle::StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5, o6) == elle::StatusError)
	escape("unable to serialize the objects");

      if (this->Sign(archive, signature) == elle::StatusError)
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
    elle::Status	Agent::Sign(const T1&			o1,
				    const T2&			o2,
				    const T3&			o3,
				    const T4&			o4,
				    const T5&			o5,
				    const T6&			o6,
				    const T7&			o7,
				    elle::Signature&		signature)
      const
    {
      elle::Archive	archive;
      elle::Digest	digest;

      enter();

      if (archive.Create() == elle::StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5, o6, o7) == elle::StatusError)
	escape("unable to serialize the objects");

      if (this->Sign(archive, signature) == elle::StatusError)
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
    elle::Status	Agent::Sign(const T1&			o1,
				    const T2&			o2,
				    const T3&			o3,
				    const T4&			o4,
				    const T5&			o5,
				    const T6&			o6,
				    const T7&			o7,
				    const T8&			o8,
				    elle::Signature&		signature)
      const
    {
      elle::Archive	archive;
      elle::Digest	digest;

      enter();

      if (archive.Create() == elle::StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4,
			    o5, o6, o7, o8) == elle::StatusError)
	escape("unable to serialize the objects");

      if (this->Sign(archive, signature) == elle::StatusError)
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
    elle::Status	Agent::Sign(const T1&			o1,
				    const T2&			o2,
				    const T3&			o3,
				    const T4&			o4,
				    const T5&			o5,
				    const T6&			o6,
				    const T7&			o7,
				    const T8&			o8,
				    const T9&			o9,
				    elle::Signature&		signature)
      const
    {
      elle::Archive	archive;
      elle::Digest	digest;

      enter();

      if (archive.Create() == elle::StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4,
			    o5, o6, o7, o8, o9) == elle::StatusError)
	escape("unable to serialize the objects");

      if (this->Sign(archive, signature) == elle::StatusError)
	escape("unable to sign the objects' archive");

      leave();
    }

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
    elle::Status	Agent::Encrypt(const T1&		o1,
				       elle::Code&		code) const
    {
      elle::Archive	archive;

      enter();

      // create an archive.
      if (archive.Create() == elle::StatusError)
	escape("unable to create the archive");

      // serialize the object.
      if (archive.Serialize(o1) == elle::StatusError)
	escape("unable to serialize the object");

      // encrypt the archive.
      if (this->Encrypt(archive, code) == elle::StatusError)
	escape("unable to encrypt the object's archive");

      leave();
    }

    template <typename T1,
	      typename T2>
    elle::Status	Agent::Encrypt(const T1&		o1,
				       const T2&		o2,
				       elle::Code&		code) const
    {
      elle::Archive	archive;

      enter();

      if (archive.Create() == elle::StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2) == elle::StatusError)
	escape("unable to serialize the object");

      if (this->Encrypt(archive, code) == elle::StatusError)
	escape("unable to encrypt the object's archive");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3>
    elle::Status	Agent::Encrypt(const T1&		o1,
				       const T2&		o2,
				       const T3&		o3,
				       elle::Code&		code) const
    {
      elle::Archive	archive;

      enter();

      if (archive.Create() == elle::StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3) == elle::StatusError)
	escape("unable to serialize the object");

      if (this->Encrypt(archive, code) == elle::StatusError)
	escape("unable to encrypt the object's archive");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4>
    elle::Status	Agent::Encrypt(const T1&		o1,
				       const T2&		o2,
				       const T3&		o3,
				       const T4&		o4,
				       elle::Code&		code) const
    {
      elle::Archive	archive;

      enter();

      if (archive.Create() == elle::StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4) == elle::StatusError)
	escape("unable to serialize the object");

      if (this->Encrypt(archive, code) == elle::StatusError)
	escape("unable to encrypt the object's archive");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5>
    elle::Status	Agent::Encrypt(const T1&		o1,
				       const T2&		o2,
				       const T3&		o3,
				       const T4&		o4,
				       const T5&		o5,
				       elle::Code&		code) const
    {
      elle::Archive	archive;

      enter();

      if (archive.Create() == elle::StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5) == elle::StatusError)
	escape("unable to serialize the object");

      if (this->Encrypt(archive, code) == elle::StatusError)
	escape("unable to encrypt the object's archive");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6>
    elle::Status	Agent::Encrypt(const T1&		o1,
				       const T2&		o2,
				       const T3&		o3,
				       const T4&		o4,
				       const T5&		o5,
				       const T6&		o6,
				       elle::Code&		code) const
    {
      elle::Archive	archive;

      enter();

      if (archive.Create() == elle::StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5, o6) == elle::StatusError)
	escape("unable to serialize the object");

      if (this->Encrypt(archive, code) == elle::StatusError)
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
    elle::Status	Agent::Encrypt(const T1&		o1,
				       const T2&		o2,
				       const T3&		o3,
				       const T4&		o4,
				       const T5&		o5,
				       const T6&		o6,
				       const T7&		o7,
				       elle::Code&		code) const
    {
      elle::Archive	archive;

      enter();

      if (archive.Create() == elle::StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5, o6, o7) == elle::StatusError)
	escape("unable to serialize the object");

      if (this->Encrypt(archive, code) == elle::StatusError)
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
    elle::Status	Agent::Encrypt(const T1&		o1,
				       const T2&		o2,
				       const T3&		o3,
				       const T4&		o4,
				       const T5&		o5,
				       const T6&		o6,
				       const T7&		o7,
				       const T8&		o8,
				       elle::Code&		code) const
    {
      elle::Archive	archive;

      enter();

      if (archive.Create() == elle::StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4,
			    o5, o6, o7, o8) == elle::StatusError)
	escape("unable to serialize the object");

      if (this->Encrypt(archive, code) == elle::StatusError)
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
    elle::Status	Agent::Encrypt(const T1&		o1,
				       const T2&		o2,
				       const T3&		o3,
				       const T4&		o4,
				       const T5&		o5,
				       const T6&		o6,
				       const T7&		o7,
				       const T8&		o8,
				       const T9&		o9,
				       elle::Code&		code) const
    {
      elle::Archive	archive;

      enter();

      if (archive.Create() == elle::StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4,
			    o5, o6, o7, o8, o9) == elle::StatusError)
	escape("unable to serialize the object");

      if (this->Encrypt(archive, code) == elle::StatusError)
	escape("unable to encrypt the object's archive");

      leave();
    }

  }
}

#endif
