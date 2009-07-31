//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/Object.cc
//
// created       julien quintard   [fri mar  6 11:37:13 2009]
// updated       julien quintard   [thu jul 30 20:38:48 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/core/Object.hh>

namespace etoile
{
  namespace core
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// this method initializes the object.
    ///
    Object::Object()
    {
      this->author.mode = Object::ModeUnknown;
      this->author.proof = NULL;

      this->data.version = 0;

      this->meta.owner.permissions = PermissionNone;
      this->meta.status.type = Object::TypeUnknown;
      this->meta.status.size = 0;
      this->meta.version = 0;
    }

    ///
    /// this method reinitializes the object.
    ///
    Object::~Object()
    {
      if (this->author.proof != NULL)
	delete this->author.proof;
    }

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
      // set the data address.
      this->data.references = references;

      // increase the data version.
      this->data.version += 1;

      // XXX[set fingerprint]

      // sign the archive with the author key.
      if (author.Sign(this->data.references,
		      this->data.fingerprint,
		      this->data.version,
		      this->data.signature) == StatusError)
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
      Address		null;

      // set the owner permissions.
      this->meta.owner.permissions = permissions;

      // set the users access address.
      this->meta.access = access;

      // increase the meta version.
      this->meta.version += 1;

      //
      // note that, if an access block is referenced, the identities and
      // permissions of the delegates must be included in the meta signature.
      //
      if (this->meta.access == null)
	{
	  // sign the meta data..
	  if (owner.Sign((Byte&)this->meta.owner.permissions,
			 this->meta.owner.token,
			 (Byte&)this->meta.status.type,
			 this->meta.status.size,
			 this->meta.access,
			 this->meta.version,
			 this->meta.signature) == StatusError)
	    escape("unable to sign the meta archive");
	}
      else
	{
	  // sign the meta fields plus the delegates' identity and
	  // permissions from the access block.

	  // XXX
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
			   this->owner.K) != StatusTrue)
	  false();
      }

      // (iii)
      {
	if (this->meta.access == null)
	  {
	    // verifye the meta part.
	    if (this->owner.K.Verify(this->meta.signature,
				     (Byte&)this->meta.owner.permissions,
				     this->meta.owner.token,
				     (Byte&)this->meta.status.type,
				     this->meta.status.size,
				     this->meta.access,
				     this->meta.version) == StatusError)
	      flee("unable to verify the meta's signature");
	  }
	else
	  {
	    // XXX
	  }
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
	      flee("unimplemented feature");
	    }
	  }
      }

      // (v)
      {
	// verify the signature.
	if (author->Verify(this->data.signature,
			   this->data.references,
			   this->data.fingerprint,
			   this->data.version) != StatusTrue)
	  flee("unable to verify the signature");
      }

      true();
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
      // call the parent class.
      if (PublicKeyBlock::Serialize(archive) == StatusError)
	escape("unable to serialize the underlying PKB");

      // serialize the owner part.
      if (archive.Serialize(this->owner.K,
			    this->owner.signature) == StatusError)
	escape("unable to serialize the owner part");

      // serialize the author part.
      if (archive.Serialize((Byte&)this->author.mode) == StatusError)
	escape("unable to serialize the author's mode");

      if (this->author.proof != NULL)
	{
	  if (archive.Serialize(*this->author.proof) == StatusError)
	    escape("unable to serialize the author's proof");
	}
      else
	{
	  // serialize 'none'.
	  if (archive.Serialize(none) == StatusError)
	    escape("unable to serialize 'none'");
	}

      // serialize the data part.
      if (archive.Serialize(this->data.references,
			    this->data.fingerprint,
			    this->data.version,
			    this->data.signature) == StatusError)
	escape("unable to serialize the data part");

      // serialize the meta part.
      if (archive.Serialize((Byte&)this->meta.owner.permissions,
			    this->meta.owner.token,
			    (Byte&)this->meta.status.type,
			    this->meta.status.size,
			    this->meta.access,
			    this->meta.version,
			    this->meta.signature) == StatusError)
	escape("unable to serialize the meta part");

      leave();
    }

    ///
    /// this method extracts the object object.
    ///
    Status		Object::Extract(Archive&		archive)
    {
      Archive::Type	type;

      // call the parent class.
      if (PublicKeyBlock::Extract(archive) == StatusError)
	escape("unable to extract the underyling PKB");

      // extract the owner part.
      if (archive.Extract(this->owner.K,
			  this->owner.signature) == StatusError)
	escape("unable to extract the owner part");

      // extract the author part.
      if (archive.Extract((Byte&)this->author.mode) == StatusError)
	escape("unable to extract the author's mode");

      // fetch the next element's type.
      if (archive.Fetch(type) == StatusError)
	escape("unable to fetch the next element's type");

      if (type == Archive::TypeNull)
	{
	  // nothing to do, keep the author's proof to NULL.
	  if (archive.Extract(none) == StatusError)
	    escape("unable to extract null");
	}
      else
	{
	  // allocate a proof.
	  this->author.proof = new Proof;

	  // extract the proof.
	  if (archive.Extract(*this->author.proof) == StatusError)
	    escape("unable to extract the author's proof");
	}

      // extract the data part.
      if (archive.Extract(this->data.references,
			  this->data.fingerprint,
			  this->data.version,
			  this->data.signature) == StatusError)
	escape("unable to extract the data part");

      // extract the meta part.
      if (archive.Extract((Byte&)this->meta.owner.permissions,
			  this->meta.owner.token,
			  (Byte&)this->meta.status.type,
			  this->meta.status.size,
			  this->meta.access,
			  this->meta.version,
			  this->meta.signature) == StatusError)
	escape("unable to extract the meta part");

      leave();
    }

  }
}
