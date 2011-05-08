//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/neutron/Object.hxx
//
// created       julien quintard   [sat may  7 22:11:14 2011]
// updated       julien quintard   [sat may  7 22:18:47 2011]
//

#ifndef NUCLEUS_NEUTRON_OBJECT_HXX
#define NUCLEUS_NEUTRON_OBJECT_HXX

namespace nucleus
{
  namespace neutron
  {

//
// ---------- templates -------------------------------------------------------
//

    ///
    /// this method seals the data and meta data by signing them.
    ///
    /// note that this method takes an undefined type because one may want
    /// to extract a token directly with a private key while another may
    /// want to rely on an external agent for instance.
    ///
    template <typename T>
    elle::Status	Object::Seal(const T&			broker,
				     const Access*		access)
    {
      enter();

      // re-sign the data if required.
      if (this->data.state == StateDirty)
	{
	  // increase the data version.
	  this->data.version += 1;

	  // sign the archive with the author key.
	  if (broker.Sign(this->data.contents,
			  this->data.size,
			  this->data.stamp,
			  this->data.fingerprint,
			  this->data.version,

			  this->meta.owner.token,
			  this->meta.access,

			  this->data.signature) == elle::StatusError)
	    escape("unable to sign the data archive");

	  // mark the section as clean.
	  this->data.state = StateClean;
	}

      // re-sign the meta data if required.
      if (this->meta.state == StateDirty)
	{

	  // increase the meta version.
	  this->meta.version += 1;

	  // perform the meta signature depending on the presence of a
	  // reference to an access block.
	  if (this->meta.access != proton::Address::Null)
	    {
	      //
	      // if an access block is referenced, the identities and
	      // permissions of the delegates must be included in the meta
	      // signature.
	      //
	      // in practical terms, a digest of the (subject, permissions)
	      // access records is computed which is then included in
	      // the meta signature.
	      //
	      elle::Digest	fingerprint;

	      // test if there is an access block.
	      if (access == NULL)
		escape("the Seal() method must take the object's "
		       "access block");

	      // compute the fingerprint of the access (subject, permissions)
	      // tuples.
	      if (access->Fingerprint(fingerprint) == elle::StatusError)
		escape("unable to compute the access block fingerprint");

	      // sign the meta data, making sure to include the access
	      // fingerprint.
	      if (broker.Sign(this->meta.owner.permissions,
			      this->meta.genre,
			      this->meta.stamp,
			      this->meta.attributes,
			      this->meta.version,

			      fingerprint,

			      this->meta.signature) == elle::StatusError)
		escape("unable to sign the meta archive");
	    }
	  else
	    {
	      //
	      // otherwise, only the meta attributes are included in
	      // the signature.
	      //

	      // sign the meta data.
	      if (broker.Sign(this->meta.owner.permissions,
			      this->meta.genre,
			      this->meta.stamp,
			      this->meta.attributes,
			      this->meta.version,

			      this->meta.signature) == elle::StatusError)
		escape("unable to sign the meta archive");
	    }

	  // mark the section as clean.
	  this->meta.state = StateClean;
	}

      leave();
    }

  }
}

#endif
