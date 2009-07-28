//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /data/mycure/repo...ries/infinit/etoile/components/Object.cc
//
// created       julien quintard   [fri mar  6 11:37:13 2009]
// updated       julien quintard   [tue jul 28 18:45:29 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/components/Object.hh>

namespace etoile
{
  namespace components
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// the class name.
    ///
    const String		Object::Class = "Object";

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates the object given the owner keypair and the
    /// type of object.
    ///
    /// the method (i) starts by initializing the underlying public key block
    /// (ii) records the owner's public key and signs it with the short-lived
    /// block private key (iii) sets the meta data, and finally (iv)
    /// initializes the data part by setting the owner as the author.
    ///
    Status		Object::Create(KeyPair&			owner,
				       Object::Type		type)
    {
      // (i)
      {
	// create the underlying public key block.
	if (PublicKeyBlock::Create() == StatusError)
	  escape("unable to create the underlying public key block");
      }

      // (ii)
      {
	// record the owner's public key and then, sign the owner's public
	// key with the PKB's private key.
	this->owner.K = owner.K;

	// since this private key will be lost soon (because it is not
	// serialized like the other attributes), the owner's private
	// key will be used on behalf of the block's private key since
	// a link is now made through this signature between the block
	// and the owner.
	if (this->k->Sign(this->owner.K, this->owner.signature) == StatusError)
	  escape("unable to sign the owner public key with the PKB "
		 "private key");
      }

      // (iii)
      {
	// set the meta type.
	this->meta.status.type = type;

	// initialize the meta data with the maximum rights for the owner and
	// a nil address for the user access block.
	if (this->Administrate(owner.k,
			       PermissionReadWrite,
			       this->meta.access) == StatusError)
	  escape("unable to set the initial meta part");
      }

      // (iv)
      {
	// write the initial data with a nil references address since no
	// data is attached to this object yet.
	//
	// note that the owner is recorded as being the initial
	// writer.
	if (this->Update(owner.k, this->data.references) == StatusError)
	  escape("unable to update the data part");
      }

      leave();
    }

    ///
    /// this method is used by the owner for updating the object's data.
    ///
    Status		Object::Update(PrivateKey&		owner,
				       Address&			references)
    {
      // set the author as being the owner.
      this->author.mode = Object::ModeOwner;

      // release the proof and re-set it to NULL because unecessary
      // in this case.
      if (this->author.proof != NULL)
	{
	  delete this->author.proof;

	  this->author.proof = NULL;
	}

      // then, write and sign the data.
      if (this->Write(owner, references) == StatusError)
	escape("unable to write the references");

      leave();
    }

    ///
    /// this method writes the data.
    ///
    Status		Object::Write(PrivateKey&		author,
				      Address&			references)
    {
      Archive		archive;

      // set the data address.
      this->data.references = references;

      // increase the data version.
      this->data.version += 1;

      // XXX[set fingerprint]

      // create an archive containing data elements so that this archive
      // can be signed.
      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      // record the elements.
      if (archive.Serialize(this->data.references,
			    this->data.fingerprint,
			    this->data.version) == StatusError)
	escape("unable to serialize the data elements");

      // sign the archive with the author key.
      if (author.Sign(archive, this->data.signature) == StatusError)
	escape("unable to sign the data archive");

      leave();
    }

    ///
    /// this method enables the owner to manage the object's meta data.
    ///
    /// the method first update the attributes before signing a
    /// temporarily created archive.
    ///
    Status		Object::Administrate(PrivateKey&	owner,
					     Permissions	permissions,
					     Address&		access)
    {
      Archive		archive;
      Address		null;

      // set the owner permissions.
      this->meta.owner.permissions = permissions;

      // set the users access address.
      this->meta.access = access;

      // increase the meta version.
      this->meta.version += 1;

      // create an archive.
      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      //
      // note that, if an access block is referenced, the identities and
      // permissions of the delegates must be included in the meta signature.
      //
      if (this->meta.access == null)
	{
	  // serialize the elements: owner-specific, status, access,
	  // version etc.
	  if (archive.Serialize((Byte&)this->meta.owner.permissions,
				this->meta.owner.token,
				(Byte&)this->meta.status.type,
				this->meta.status.size,
				this->meta.access,
				this->meta.version) == StatusError)
	    escape("unable to serialize the meta part");
	}
      else
	{
	  // serialize the meta fields plus the delegates' identity and
	  // permissions from the access block.

	  // XXX
	}

      // sign the meta data.
      if (owner.Sign(archive, this->meta.signature) == StatusError)
	escape("unable to sign the meta archive");

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
    Status		Object::Validate(Address&		address)
    {
      Address		null;
      PublicKey*	author;

      // (i)
      {
	// call the parent class.
	if (PublicKeyBlock::Validate(address) == StatusFalse)
	  false();
      }

      // (ii)
      {
	// verify the owner's key signature with the block's public key.
	if (this->K.Verify(this->owner.signature,
			   this->owner.K) == StatusFalse)
	  false();
      }

      // (iii)
      {
	Archive		archive;

	// create an archive.
	if (archive.Create() == StatusError)
	  flee("unable to create an archive");

	// serialize the meta part depending on the presence of a
	// access block.
	if (this->meta.access == null)
	  {
	    if (archive.Serialize((Byte&)this->meta.owner.permissions,
				  this->meta.owner.token,
				  (Byte&)this->meta.status.type,
				  this->meta.status.size,
				  this->meta.access,
				  this->meta.version) == StatusError)
	      flee("unable to serialize the meta part");
	  }
	else
	  {
	    // XXX
	  }

	// verify the meta signature.
	if (this->owner.K.Verify(this->meta.signature,
				 archive) == StatusError)
	  false();
      }

      // (iv)
      {
	switch (this->author.mode)
	  {
	  case Object::ModeOwner:
	    {
	      author = &this->owner.K;

	      break;
	    }
	  default:
	    {
	      escape("unimplemented feature");
	    }
	  }
      }

      // (v)
      {
	Archive		archive;

	// create an archive.
	if (archive.Create() == StatusError)
	  escape("unable to create the archive");

	// record the elements.
	if (archive.Serialize(this->data.references,
			      this->data.fingerprint,
			      this->data.version) == StatusError)
	  escape("unable to serialize the data elements");

	// verify the signature.
	if (author->Verify(this->data.signature,
			   archive) != StatusTrue)
	  escape("unable to verify the signature");
      }

      true();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this method initializes the object.
    ///
    Status		Object::New(Object&			object)
    {
      object.author.mode = Object::ModeUnknown;
      object.author.proof = NULL;

      object.data.version = 0;

      object.meta.owner.permissions = PermissionNone;
      object.meta.status.type = Object::TypeUnknown;
      object.meta.status.size = 0;
      object.meta.version = 0;

      leave();
    }

    ///
    /// this method reinitializes the object.
    ///
    Status		Object::Delete(Object&			object)
    {
      if (object.author.proof != NULL)
	delete object.author.proof;

      leave();
    }

    ///
    /// assign the object.
    ///
    Object&		Object::operator=(const Object&		element)
    {
      // self-check.
      if (this == &element)
	return (*this);

      // call the parent class.
      PublicKeyBlock::operator=(element);

      // reinitialize the object.
      if ((Object::Delete(*this) == StatusError) ||
	  (Object::New(*this) == StatusError))
	yield("unable to reinitialize the object", *this);

      // assign the attributes.
      this->owner.K = element.owner.K;
      this->owner.signature = element.owner.signature;

      this->author.mode = element.author.mode;
      if (element.author.proof != NULL)
	{
	  this->author.proof = new Proof;

	  *this->author.proof = *element.author.proof;
	}

      this->data.references = element.data.references;
      this->data.fingerprint = element.data.fingerprint;
      this->data.version = element.data.version;
      this->data.signature = element.data.signature;

      this->meta.owner.permissions = element.meta.owner.permissions;
      this->meta.owner.token = element.meta.owner.token;
      this->meta.status.type = element.meta.status.type;
      this->meta.status.size = element.meta.status.size;
      this->meta.access = element.meta.access;
      this->meta.version = element.meta.version;
      this->meta.signature = element.meta.signature;

      return (*this);
    }

    ///
    /// this method checks if two objects match.
    ///
    Boolean		Object::operator==(const Object&	element)
    {
      // call the parent class.
      if (PublicKeyBlock::operator==(element) == false)
	false();

      // compare the owner part.
      if ((this->owner.K != element.owner.K) ||
	  (this->owner.signature != element.owner.signature))
	false();

      // compare the author part.
      if (this->author.mode != element.author.mode)
	false;

      if ((this->author.proof == NULL) || (element.author.proof == NULL))
	{
	  if (this->author.proof != element.author.proof)
	    false();
	}
      else
	{
	  if (*this->author.proof != *element.author.proof)
	    false();
	}

      // compare the data part.
      if ((this->data.references != element.data.references) ||
	  (this->data.fingerprint != element.data.fingerprint) ||
	  (this->data.version != element.data.version) ||
	  (this->data.signature != element.data.signature))
	false();

      // compare the meta part.
      if ((this->meta.owner.permissions != element.meta.owner.permissions) ||
	  (this->meta.owner.token != element.meta.owner.token) ||
	  (this->meta.status.type != element.meta.status.type) ||
	  (this->meta.status.size != element.meta.status.size) ||
	  (this->meta.access != element.meta.access) ||
	  (this->meta.version != element.meta.version) ||
	  (this->meta.signature != element.meta.signature))
	false();

      true();
    }

    ///
    /// this method checks if two objects dis-match.
    ///
    Boolean		Object::operator!=(const Object&	element)
    {
      return (!(*this == element));
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an object object.
    ///
    Status		Object::Dump(Natural32			margin)
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');

      std::cout << alignment << "[Object]" << std::endl;

      // dump the parent class.
      if (PublicKeyBlock::Dump(margin + 2) == StatusError)
	escape("unable to dump the underlying public key block");

      // dump the owner part.
      std::cout << alignment << shift << "[Owner]" << std::endl;

      std::cout << alignment << shift << shift << "[K]" << std::endl;
      if (this->owner.K.Dump(margin + 6) == StatusError)
	escape("unable to dump the owner's public key");

      std::cout << alignment << shift << shift << "[Signature]" << std::endl;
      if (this->owner.signature.Dump(margin + 6) == StatusError)
	escape("unable to dump the owner's signature");

      // dump the author part.
      std::cout << alignment << shift << "[Author]" << std::endl;

      std::cout << alignment << shift << shift << "[Mode] "
		<< this->author.mode << std::endl;

      if (this->author.proof != NULL)
	{
	  std::cout << alignment << shift << shift << "[Proof]"
		    << std::endl;
	  if (this->author.proof->Dump(margin + 6) == StatusError)
	    escape("unable to dump the proof");
	}
      else
	{
	  std::cout << alignment << shift << shift << "[Proof] "
		    << none << std::endl;
	}

      // dump the data part.
      std::cout << alignment << shift << "[Data]" << std::endl;

      std::cout << alignment << shift << shift << "[References]" << std::endl;
      if (this->data.references.Dump(margin + 6) == StatusError)
	escape("unable to dump the references' address");

      std::cout << alignment << shift << shift << "[Fingerprint]" << std::endl;
      if (this->data.fingerprint.Dump(margin + 6) == StatusError)
	escape("unable to dump the fingerprint");

      std::cout << alignment << shift << shift << "[Version] "
		<< this->data.version << std::endl;

      std::cout << alignment << shift << shift << "[Signature]" << std::endl;
      if (this->data.signature.Dump(margin + 6) == StatusError)
	escape("unable to dump the data signature");

      // dump the meta part.
      std::cout << alignment << shift << "[Meta]" << std::endl;

      std::cout << alignment << shift << shift << "[Owner] " << std::endl;

      std::cout << alignment << shift << shift << shift << "[Permissions] "
		<< this->meta.owner.permissions << std::endl;

      std::cout << alignment << shift << shift << shift << "[Token]"
		<< std::endl;
      if (this->meta.owner.token.Dump(margin + 8) == StatusError)
	escape("unable to dump the meta owner's token");

      std::cout << alignment << shift << shift << "[Status] " << std::endl;

      std::cout << alignment << shift << shift << shift << "[Type] "
		<< this->meta.status.type << std::endl;
      std::cout << alignment << shift << shift << shift << "[Size] "
		<< this->meta.status.size << std::endl;

      std::cout << alignment << shift << shift << "[Access]" << std::endl;
      if (this->meta.access.Dump(margin + 6) == StatusError)
	escape("unable to dump the meta access address");

      std::cout << alignment << shift << shift << "[Version] "
		<< this->meta.version << std::endl;

      std::cout << alignment << shift << shift << "[Signature]" << std::endl;
      if (this->meta.signature.Dump(margin + 6) == StatusError)
	escape("unable to dump the meta signature");

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the object object.
    ///
    Status		Object::Serialize(Archive&		archive) const
    {
      Archive		ar;

      // call the parent class.
      if (PublicKeyBlock::Serialize(archive) == StatusError)
	escape("unable to serialize the underlying PKB");

      // prepare the object archive.
      if (ar.Create() == StatusError)
	escape("unable to prepare the object archive");

      // serialize the class name.
      if (ar.Serialize(Object::Class) == StatusError)
	escape("unable to serialize the class name");

      // serialize the owner part.
      if (ar.Serialize(this->owner.K,
		       this->owner.signature) == StatusError)
	escape("unable to serialize the owner part");

      // serialize the author part.
      if (ar.Serialize((Byte&)this->author.mode) == StatusError)
	escape("unable to serialize the author's mode");

      if (this->author.proof != NULL)
	{
	  if (ar.Serialize(*this->author.proof) == StatusError)
	    escape("unable to serialize the author's proof");
	}
      else
	{
	  // serialize 'none'.
	  if (ar.Serialize(none) == StatusError)
	    escape("unable to serialize 'none'");
	}

      // serialize the data part.
      if (ar.Serialize(this->data.references,
		       this->data.fingerprint,
		       this->data.version,
		       this->data.signature) == StatusError)
	escape("unable to serialize the data part");

      // serialize the meta part.
      if (ar.Serialize((Byte&)this->meta.owner.permissions,
		       this->meta.owner.token,
		       (Byte&)this->meta.status.type,
		       this->meta.status.size,
		       this->meta.access,
		       this->meta.version,
		       this->meta.signature) == StatusError)
	escape("unable to serialize the meta part");

      // record the object archive into the given archive.
      if (archive.Serialize(ar) == StatusError)
	escape("unable to serialize the object archive");

      leave();
    }

    ///
    /// this method extracts the object object.
    ///
    Status		Object::Extract(Archive&		archive)
    {
      Archive		ar;
      String		name;
      Archive::Type	type;

      // call the parent class.
      if (PublicKeyBlock::Extract(archive) == StatusError)
	escape("unable to extract the underyling PKB");

      // extract the object archive object.
      if (archive.Extract(ar) == StatusError)
	escape("unable to extract the object archive object");

      // extract the name.
      if (ar.Extract(name) == StatusError)
	escape("unable to extract the class name");

      // extract the owner part.
      if (ar.Extract(this->owner.K,
		     this->owner.signature) == StatusError)
	escape("unable to extract the owner part");

      // extract the author part.
      if (ar.Extract((Byte&)this->author.mode) == StatusError)
	escape("unable to extract the author's mode");

      // fetch the next element's type.
      if (ar.Fetch(type) == StatusError)
	escape("unable to fetch the next element's type");

      if (type == Archive::TypeNull)
	{
	  // nothing to do, keep the author's proof to NULL.
	  if (ar.Extract(none) == StatusError)
	    escape("unable to extract null");
	}
      else
	{
	  // allocate a proof.
	  this->author.proof = new Proof;

	  // extract the proof.
	  if (ar.Extract(*this->author.proof) == StatusError)
	    escape("unable to extract the author's proof");
	}

      // extract the data part.
      if (ar.Extract(this->data.references,
		     this->data.fingerprint,
		     this->data.version,
		     this->data.signature) == StatusError)
	escape("unable to extract the data part");

      // extract the meta part.
      if (ar.Extract((Byte&)this->meta.owner.permissions,
		     this->meta.owner.token,
		     (Byte&)this->meta.status.type,
		     this->meta.status.size,
		     this->meta.access,
		     this->meta.version,
		     this->meta.signature) == StatusError)
	escape("unable to extract the meta part");

      // check the name.
      if (Object::Class != name)
	escape("wrong class name in the extract object");

      leave();
    }

  }
}
