//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/Object.cc
//
// created       julien quintard   [fri mar  6 11:37:13 2009]
// updated       julien quintard   [fri may 28 19:10:59 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/kernel/Object.hh>

#include <etoile/hole/Component.hh>

#include <etoile/user/Agent.hh>

namespace etoile
{
  namespace kernel
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// the component identifier.
    ///
    const elle::String			Object::Name = "Object";

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// this method initializes the object.
    ///
    Object::Object():
      PublicKeyBlock::PublicKeyBlock(hole::ComponentObject)
    {
      //
      // the attributes below are initialized in the constructor body
      // because they belong to a sub-structure.
      //
      this->meta.state = StateClean;
      this->meta.owner.permissions = PermissionNone;
      this->meta.genre = GenreUnknown;
      this->meta.version = 0;

      this->data.state = StateClean;
      this->data.size = 0;
      this->data.version = 0;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates the object given the owner public and the
    /// genre of the object to create.
    ///
    /// the method (i) starts by initializing the underlying public key block
    /// (ii) records the owner's public key and signs it with the short-lived
    /// block private key (iii) sets the meta data, and finally (iv)
    /// initializes the data part by setting the owner as the author.
    ///
    elle::Status	Object::Create(const Genre		genre,
				       const elle::PublicKey&	owner)
    {
      elle::KeyPair	pair;

      enter();

      // (i)
      {
	// generate a key pair for the PKB.
	if (pair.Generate() == elle::StatusError)
	  escape("unable to generate a key pair");

	// create the underlying public key block.
	if (PublicKeyBlock::Create(pair.K) == elle::StatusError)
	  escape("unable to create the underlying public key block");
      }

      // (ii)
      {
	// record the owner's public key and then, sign the owner's public
	// key with the PKB's private key.
	this->owner.K = owner;

	// since this private key will be lost soon (because it is not
	// serialized like the other attributes), the owner's private
	// key will be used on behalf of the block's private key since
	// a link is now made through this signature between the block
	// and the owner.
	if (pair.k.Sign(this->owner.K, this->owner.signature) ==
	    elle::StatusError)
	  escape("unable to sign the owner public key with the PKB "
		 "private key");

	// create a subject corresponding to the user. note that this
	// subject will never be serialized hence is not really part of
	// the object but is used to ease the process of access control.
	if (this->owner.subject.Create(this->owner.K) == elle::StatusError)
	  escape("unable to create the owner subject");
      }

      // (iii)
      {
	// set the meta genre.
	this->meta.genre = genre;

	// set the initial owner permissions to all with an empty key.
	if (this->Administrate(this->meta.attributes,
			       hole::Address::Null,
			       PermissionReadWrite,
			       this->meta.owner.token) == elle::StatusError)
	  escape("unable to set the initial meta data");
      }

      // (iv)
      {
	elle::Digest	fingerprint;
	Author		author;

	// set an owner author.
	if (author.Create() == elle::StatusError)
	  escape("unable to create an author");

	// set the initial data with no contents, an empty fingerprint and
	// the owner as the author.
	if (this->Update(author,
			 hole::Address::Null,
			 0,
			 fingerprint) == elle::StatusError)
	  escape("unable to set the initial data");
      }

      leave();
    }

    ///
    /// this method updates the data section.
    ///
    elle::Status	Object::Update(const Author&		author,
				       const hole::Address&	contents,
				       const Size&		size,
				       const elle::Digest&	fingerprint)
    {
      enter();

      // set the last update time.
      if (this->data.stamp.Current() == elle::StatusError)
	escape("unable to set the last update time");

      // set the author.
      this->author = author;

      // set the address.
      this->data.contents = contents;

      // set the size.
      this->data.size = size;

      // set the fingerprint.
      this->data.fingerprint = fingerprint;

      // mark the section as dirty.
      this->data.state = StateDirty;

      leave();
    }

    ///
    /// this method updates the meta section.
    ///
    elle::Status	Object::Administrate(const Attributes&	attributes,
					     const hole::Address& access,
					     const Permissions&	permissions,
					     const Token&	token)
    {
      enter();

      // check if the access block address of the owner's token have changed.
      if ((this->meta.access != access) ||
	  (this->meta.owner.token != token))
	{
	  //
	  // in this case, the data section will have to be re-signed.
	  //
	  // indeed, the access address and owner token are actually
	  // included in the data section.
	  //
	  // for more information, please refer to the research publications.
	  //

	  // set the address.
	  this->meta.access = access;

	  // set the owner token.
	  this->meta.owner.token = token;

	  // set the data section as dirty.
	  this->data.state = StateDirty;
	}

      //
      // in any case, since the object has been administered, the management
      // time is going to be re-calculated, hence the meta section will
      // change, therefore it will need to be re-signed.
      //
      // the following simply assign the meta attributes and set the
      // meta section as dirty.
      //

      // set the last management time.
      if (this->meta.stamp.Current() == elle::StatusError)
	escape("unable to set the last management time");

      // set the attributes.
      this->meta.attributes = attributes;

      // set the owner permissions.
      this->meta.owner.permissions = permissions;

      // mark the section as dirty.
      this->meta.state = StateDirty;

      // re-compute the owner's access record. just like this->owner.subject,
      // this attribute is not mandatory but has been introduced in order
      // to simplify access control management.
      if (this->meta.owner.record.Update(this->owner.subject,
					 this->meta.owner.permissions,
					 this->meta.owner.token) ==
	  elle::StatusError)
	escape("unable to create the owner access record");

      leave();
    }

    ///
    /// this method seals the data and meta data by signing them.
    ///
    elle::Status	Object::Seal(const user::Agent&		agent,
				     const Access*		access)
    {
      enter();

      // re-sign the data if required.
      if (this->data.state == StateDirty)
	{
	  // increase the data version.
	  this->data.version += 1;

	  // sign the archive with the author key.
	  if (agent.Sign(this->data.contents,
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
	  if (this->meta.access != hole::Address::Null)
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
	      if (agent.Sign(this->meta.owner.permissions,
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
	      if (agent.Sign(this->meta.owner.permissions,
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

    ///
    /// this method verifies that the object has been properly created
    /// i.e that every signature has been produced by legitimate users.
    ///
    /// the method (i) calls the parent class for validation (ii)
    /// verifies the owner part's signature (iii) verifies the meta part's
    /// signature (iv) retrieves the author's public key (v) verifies
    /// the data signature.
    ///
    elle::Status	Object::Validate(const hole::Universe&	universe,
					 const hole::Address&	address,
					 const Access*		access)
      const
    {
      const elle::PublicKey*	author;

      enter();

      // (i)
      {
	// call the parent class.
	if (PublicKeyBlock::Validate(universe, address) == elle::StatusFalse)
	  flee("unable to verify the underlying PKB");
      }

      // (ii)
      {
	// verify the owner's key signature with the block's public key.
	if (this->K.Verify(this->owner.signature,
			   this->owner.K) != elle::StatusTrue)
	  flee("unable to verify the owner's signature");
      }

      // (iii)
      {
	if (this->meta.access != hole::Address::Null)
	  {
	    elle::Digest	fingerprint;

	    // test if there is an access block.
	    if (access == NULL)
	      escape("the Seal() method must take the object's access block");

	    // compute the fingerprint of the access (subject, permissions)
	    // tuples.
	    if (access->Fingerprint(fingerprint) == elle::StatusError)
	      escape("unable to compute the access block fingerprint");

	    // verify the meta part, including the access fingerprint.
	    if (this->owner.K.Verify(this->meta.signature,

				     this->meta.owner.permissions,
				     this->meta.genre,
				     this->meta.stamp,
				     this->meta.attributes,
				     this->meta.version,

				     fingerprint) == elle::StatusError)
	      flee("unable to verify the meta's signature");
	  }
	else
	  {
	    // verify the meta part.
	    if (this->owner.K.Verify(this->meta.signature,

				     this->meta.owner.permissions,
				     this->meta.genre,
				     this->meta.stamp,
				     this->meta.attributes,
				     this->meta.version) == elle::StatusError)
	      flee("unable to verify the meta's signature");
	  }
      }

      // (iv)
      {
	switch (this->author.role)
	  {
	  case RoleOwner:
	    {
	      author = &this->owner.K;

	      break;
	    }
	  default:
	    {
	      // XXX
	      printf("UNIMPLEMENTED\n");	
	    }
	  }
      }

      // (v)
      {
	// verify the signature.
	if (author->Verify(this->data.signature,

			   this->data.contents,
			   this->data.size,
			   this->data.stamp,
			   this->data.fingerprint,
			   this->data.version,

			   this->meta.owner.token,
			   this->meta.access) != elle::StatusTrue)
	  flee("unable to verify the data signature");
      }

      true();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the object.
    ///
    embed(Object, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an object object.
    ///
    elle::Status	Object::Dump(elle::Natural32		margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Object]" << std::endl;

      // dump the parent class.
      if (PublicKeyBlock::Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the underlying public key block");

      // dump the owner part.
      std::cout << alignment << elle::Dumpable::Shift << "[Owner]"
		<< std::endl;

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
		<< "[K]" << std::endl;
      if (this->owner.K.Dump(margin + 6) == elle::StatusError)
	escape("unable to dump the owner's public key");

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
		<< "[Signature]" << std::endl;
      if (this->owner.signature.Dump(margin + 6) == elle::StatusError)
	escape("unable to dump the owner's signature");

      // dump the author part.
      if (this->author.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the author");

      // dump the meta part.
      std::cout << alignment << elle::Dumpable::Shift << "[Meta]" << std::endl;

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
		<< "[Owner] " << std::endl;

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
		<< elle::Dumpable::Shift << "[Permissions] "
		<< (elle::Natural32)this->meta.owner.permissions << std::endl;

      if (this->meta.owner.token.Dump(margin + 6) == elle::StatusError)
	escape("unable to dump the meta owner's token");

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
		<< "[Genre] " << (elle::Natural32)this->meta.genre
		<< std::endl;

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
		<< "[Stamp] " << std::endl;
      if (this->meta.stamp.Dump(margin + 6) == elle::StatusError)
	escape("unable to dump the meta stamp");

      if (this->meta.attributes.Dump(margin + 4) == elle::StatusError)
	escape("unable to dump the meta attributess");

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
		<< "[Access]" << std::endl;
      if (this->meta.access.Dump(margin + 6) == elle::StatusError)
	escape("unable to dump the meta access address");

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
		<< "[Version] " << this->meta.version << std::endl;

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
		<< "[Signature]" << std::endl;
      if (this->meta.signature.Dump(margin + 6) == elle::StatusError)
	escape("unable to dump the meta signature");

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
		<< "[State] " << this->meta.state << std::endl;

      // dump the data part.
      std::cout << alignment << elle::Dumpable::Shift << "[Data]" << std::endl;

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
		<< "[Contents]" << std::endl;
      if (this->data.contents.Dump(margin + 6) == elle::StatusError)
	escape("unable to dump the contents' address");

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
		<< "[Size] " << this->data.size << std::endl;

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
		<< "[Stamp]" << std::endl;
      if (this->data.stamp.Dump(margin + 6) == elle::StatusError)
	escape("unable to dump the data stamp");

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
		<< "[Fingerprint]" << std::endl;
      if (this->data.fingerprint.Dump(margin + 6) == elle::StatusError)
	escape("unable to dump the fingerprint");

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
		<< "[Version] " << this->data.version << std::endl;

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
		<< "[Signature]" << std::endl;
      if (this->data.signature.Dump(margin + 6) == elle::StatusError)
	escape("unable to dump the data signature");

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
		<< "[State] " << this->data.state << std::endl;

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the object object.
    ///
    elle::Status	Object::Serialize(elle::Archive&	archive) const
    {
      enter();

      // serialize the component name.
      if (archive.Serialize(Object::Name) == elle::StatusError)
	escape("unable to serialize the component identifier");

      // call the parent class.
      if (PublicKeyBlock::Serialize(archive) == elle::StatusError)
	escape("unable to serialize the underlying PKB");

      // serialize the owner part.
      if (archive.Serialize(this->owner.K,
			    this->owner.signature) == elle::StatusError)
	escape("unable to serialize the owner part");

      // serialize the author part.
      if (archive.Serialize(this->author) == elle::StatusError)
	escape("unable to serialize the author");

      // serialize the meta part.
      if (archive.Serialize(this->meta.owner.permissions,
			    this->meta.owner.token,
			    this->meta.genre,
			    this->meta.stamp,
			    this->meta.attributes,
			    this->meta.access,
			    this->meta.version,
			    this->meta.signature) == elle::StatusError)
	escape("unable to serialize the meta part");

      // serialize the data part.
      if (archive.Serialize(this->data.contents,
			    this->data.size,
			    this->data.stamp,
			    this->data.fingerprint,
			    this->data.version,
			    this->data.signature) == elle::StatusError)
	escape("unable to serialize the data part");

      leave();
    }

    ///
    /// this method extracts the object object.
    ///
    elle::Status	Object::Extract(elle::Archive&		archive)
    {
      enter();

      // call the parent class.
      if (PublicKeyBlock::Extract(archive) == elle::StatusError)
	escape("unable to extract the underyling PKB");

      // extract the owner part.
      if (archive.Extract(this->owner.K,
			  this->owner.signature) == elle::StatusError)
	escape("unable to extract the owner part");

      // extract the author part.
      if (archive.Extract(this->author) == elle::StatusError)
	escape("unable to extract the author");

      // extract the meta part.
      if (archive.Extract(this->meta.owner.permissions,
			  this->meta.owner.token,
			  this->meta.genre,
			  this->meta.stamp,
			  this->meta.attributes,
			  this->meta.access,
			  this->meta.version,
			  this->meta.signature) == elle::StatusError)
	escape("unable to extract the meta part");

      // extract the data part.
      if (archive.Extract(this->data.contents,
			  this->data.size,
			  this->data.stamp,
			  this->data.fingerprint,
			  this->data.version,
			  this->data.signature) == elle::StatusError)
	escape("unable to extract the data part");

      // compute the owner subject.
      if (this->owner.subject.Create(this->owner.K) == elle::StatusError)
	escape("unable to create the owner subject");

      // compute the owner record.
      if (this->meta.owner.record.Update(this->owner.subject,
					 this->meta.owner.permissions,
					 this->meta.owner.token) ==
	  elle::StatusError)
	escape("unable to create the owner access record");

      leave();
    }

  }
}
