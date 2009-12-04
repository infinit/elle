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
// updated       julien quintard   [tue dec  1 03:01:50 2009]
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
    Object::Object():
      PublicKeyBlock(hole::Block::KindObject)
    {
      this->meta.state = StateClean;
      this->meta.owner.permissions = PermissionNone;
      this->meta.status.genre = GenreUnknown;
      this->meta.version = 0;

      this->data.state = StateClean;
      this->data.size = 0;
      this->data.version = 0;
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
    Status		Object::Create(const Genre		genre,
				       const PublicKey&		owner)
    {
      KeyPair		pair;

      // (i)
      {
	// generate a key pair for the PKB.
	if (pair.Generate() == StatusError)
	  escape("unable to generate a key pair");

	// create the underlying public key block.
	if (PublicKeyBlock::Create(pair) == StatusError)
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
	if (pair.k.Sign(this->owner.K, this->owner.signature) == StatusError)
	  escape("unable to sign the owner public key with the PKB "
		 "private key");
      }

      // (iii)
      {
	// set the meta genre.
	this->meta.status.genre = genre;

	// set the initial owner permissions to all.
	if (this->Administrate(hole::Address::Null,
			       PermissionReadWrite) == StatusError)
	  escape("unable to set the initial meta data");
      }

      // (iv)
      {
	Digest		fingerprint;
	Author		author;

	// set an owner author.
	if (author.Create() == StatusError)
	  escape("unable to create an author");

	// set the initial data with no contents, an empty fingerprint and
	// the owner as the author.
	if (this->Update(author,
			 hole::Address::Null,
			 0,
			 fingerprint) == StatusError)
	  escape("unable to set the initial data");
      }

      leave();
    }

    ///
    /// this method updates the data section.
    ///
    Status		Object::Update(const Author&		author,
				       const hole::Address&	contents,
				       const Contents::Offset&	size,
				       const Digest&		fingerprint)
    {
      // set the author.
      this->author = author;

      // set the address.
      this->data.contents = contents;

      // set the size.
      this->data.size = size;

      // set the fingerprint.
      this->data.fingerprint = fingerprint;

      // set the last update time.
      if (this->data.stamp.Current() == StatusError)
	escape("unable to set the last update time");

      // mark the section as dirty.
      this->data.state = StateDirty;

      leave();
    }

    ///
    /// this method updates the meta section.
    ///
    Status		Object::Administrate(const hole::Address& access,
					     const Permissions&	permissions)
    {
      // set the address.
      this->meta.access = access;

      // set the owner permissions.
      this->meta.owner.permissions = permissions;

      // set the last management time.
      if (this->meta.status.stamp.Current() == StatusError)
	escape("unable to set the last management time");

      // mark the section as dirty.
      this->meta.state = StateDirty;

      leave();
    }

    ///
    /// this method seals the data and meta data by signing them.
    ///
    Status		Object::Seal(const PrivateKey&		user)
    {
      // re-sign the data if required.
      if (this->data.state == StateDirty)
	{
	  // increase the data version.
	  this->data.version += 1;

	  // sign the archive with the author key.
	  if (user.Sign(this->data.contents,
			this->data.size,
			this->data.stamp,
			this->data.fingerprint,
			this->data.version,
			this->data.signature) == StatusError)
	    escape("unable to sign the data archive");

	  // mark the section as clean.
	  this->data.state = StateClean;
	}

      // re-sign the meta data if required.
      if (this->meta.state == StateDirty)
	{
	  // increase the meta version.
	  this->meta.version += 1;

	  //
	  // note that, if an access block is referenced, the identities and
	  // permissions of the delegates must be included in the meta
	  // signature.
	  //
	  if (this->meta.access == hole::Address::Null)
	    {
	      // sign the meta data..
	      if (user.Sign((Byte&)this->meta.owner.permissions,
			    this->meta.owner.token,
			    (Byte&)this->meta.status.genre,
			    this->meta.status.stamp,
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
	      printf("NOT IMPLEMENTED YET\n");
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
    Status		Object::Validate(const hole::Address&	address)
      const
    {
      const PublicKey*	author;

      // (i)
      {
	// call the parent class.
	if (PublicKeyBlock::Validate(address) == StatusFalse)
	  flee("unable to verify the underlying PKB");
      }

      // (ii)
      {
	// verify the owner's key signature with the block's public key.
	if (this->K.Verify(this->owner.signature,
			   this->owner.K) != StatusTrue)
	  flee("unable to verify the owner's signature");
      }

      // (iii)
      {
	if (this->meta.access == hole::Address::Null)
	  {
	    // verifye the meta part.
	    if (this->owner.K.Verify(this->meta.signature,
				     (Byte&)this->meta.owner.permissions,
				     this->meta.owner.token,
				     (Byte&)this->meta.status.genre,
				     this->meta.status.stamp,
				     this->meta.access,
				     this->meta.version) == StatusError)
	      flee("unable to verify the meta's signature");
	  }
	else
	  {
	    // XXX
	    printf("UNIMPLEMENTED\n");
	  }
      }

      // (iv)
      {
	switch (this->author.mode)
	  {
	  case Author::ModeOwner:
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
			   this->data.version) != StatusTrue)
	  flee("unable to verify the data signature");
      }

      true();
    }

//
// ---------- entity ----------------------------------------------------------
//

    ///
    /// assign the object.
    ///
    Object&		Object::operator=(const Object&		element)
    {
      // self-check.
      if (this == &element)
	return (*this);

      // recycle the address.
      if (this->Recycle<Object>(&element) == StatusError)
	yield("unable to recycle the object", *this);

      return (*this);
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an object object.
    ///
    Status		Object::Dump(Natural32			margin) const
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
      if (this->author.Dump(margin + 2) == StatusError)
	escape("unable to dump the author");

      // dump the meta part.
      std::cout << alignment << shift << "[Meta]" << std::endl;

      std::cout << alignment << shift << shift << "[Owner] " << std::endl;

      std::cout << alignment << shift << shift << shift << "[Permissions] "
		<< this->meta.owner.permissions << std::endl;

      if (this->meta.owner.token.Dump(margin + 6) == StatusError)
	escape("unable to dump the meta owner's token");

      std::cout << alignment << shift << shift << "[Status] " << std::endl;

      std::cout << alignment << shift << shift << shift << "[Genre] "
		<< this->meta.status.genre << std::endl;

      std::cout << alignment << shift << shift << shift << "[Stamp] "
		<< std::endl;
      if (this->meta.status.stamp.Dump(margin + 8) == StatusError)
	escape("unable to dump the meta stamp");

      std::cout << alignment << shift << shift << "[Access]" << std::endl;
      if (this->meta.access.Dump(margin + 6) == StatusError)
	escape("unable to dump the meta access address");

      std::cout << alignment << shift << shift << "[Version] "
		<< this->meta.version << std::endl;

      std::cout << alignment << shift << shift << "[Signature]" << std::endl;
      if (this->meta.signature.Dump(margin + 6) == StatusError)
	escape("unable to dump the meta signature");

      std::cout << alignment << shift << shift << "[State] "
		<< this->meta.state << std::endl;

      // dump the data part.
      std::cout << alignment << shift << "[Data]" << std::endl;

      std::cout << alignment << shift << shift << "[Contents]" << std::endl;
      if (this->data.contents.Dump(margin + 6) == StatusError)
	escape("unable to dump the contents' address");

      std::cout << alignment << shift << shift << "[Size] "
		<< this->data.size << std::endl;

      std::cout << alignment << shift << shift << "[Stamp]" << std::endl;
      if (this->data.stamp.Dump(margin + 6) == StatusError)
	escape("unable to dump the data stamp");

      std::cout << alignment << shift << shift << "[Fingerprint]" << std::endl;
      if (this->data.fingerprint.Dump(margin + 6) == StatusError)
	escape("unable to dump the fingerprint");

      std::cout << alignment << shift << shift << "[Version] "
		<< this->data.version << std::endl;

      std::cout << alignment << shift << shift << "[Signature]" << std::endl;
      if (this->data.signature.Dump(margin + 6) == StatusError)
	escape("unable to dump the data signature");

      std::cout << alignment << shift << shift << "[State] "
		<< this->data.state << std::endl;

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
      if (archive.Serialize(this->author) == StatusError)
	escape("unable to serialize the author");

      // serialize the meta part.
      if (archive.Serialize((Byte&)this->meta.owner.permissions,
			    this->meta.owner.token,
			    (Byte&)this->meta.status.genre,
			    this->meta.status.stamp,
			    this->meta.access,
			    this->meta.version,
			    this->meta.signature) == StatusError)
	escape("unable to serialize the meta part");

      // serialize the data part.
      if (archive.Serialize(this->data.contents,
			    this->data.size,
			    this->data.stamp,
			    this->data.fingerprint,
			    this->data.version,
			    this->data.signature) == StatusError)
	escape("unable to serialize the data part");

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
      if (archive.Extract(this->author) == StatusError)
	escape("unable to extract the author");

      // extract the meta part.
      if (archive.Extract((Byte&)this->meta.owner.permissions,
			  this->meta.owner.token,
			  (Byte&)this->meta.status.genre,
			  this->meta.status.stamp,
			  this->meta.access,
			  this->meta.version,
			  this->meta.signature) == StatusError)
	escape("unable to extract the meta part");

      // extract the data part.
      if (archive.Extract(this->data.contents,
			  this->data.size,
			  this->data.stamp,
			  this->data.fingerprint,
			  this->data.version,
			  this->data.signature) == StatusError)
	escape("unable to extract the data part");

      leave();
    }

  }
}
