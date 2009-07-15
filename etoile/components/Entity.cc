//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       GPL
//
// file          /home/mycure/infinit/infinit/components/Entity.cc
//
// created       julien quintard   [fri mar  6 11:37:13 2009]
// updated       julien quintard   [wed mar 11 16:55:15 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <infinit/components/Entity.hh>

namespace infinit
{
  namespace components
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// the class name.
    ///
    const String		Entity::Class = "Entity";

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates the entity given the owner keypair and the
    /// type of entity.
    ///
    /// the method (i) starts by initializing the underlying public key block
    /// before (ii) recording the owner's public key and signing it with
    /// the short-lived block private key, (iii) initializing the data
    /// part by making the entity owner write it and (iv) setting the
    /// meta data and finally signing them.
    ///
    Status		Entity::Create(KeyPair&			owner,
				       Entity::Type		type)
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
	  escape("unable to sign the owner public key with the PKB private key");
      }

      // (iii)
      {
	// write the initial contents with a nil data address
	// since no data is present in this entity yet.
	//
	// note that the owner is recorded as being the initial
	// writer.
	if (this->Update(owner.k, this->contents.data) == StatusError)
	  escape("unable to update the contents part");
      }

      // (iv)
      {
	// set the meta type.
	this->meta.type = type;

	// call the Manage() method to initialize the meta data
	// with the maximum rights for the owner and a nil address
	// for the user access block.
	if (this->Manage(owner.k,
			 Access::RightWrite,
			 this->meta.access) == StatusError)
	  escape("unable to set the initial meta part");
      }

      leave();
    }

    ///
    /// this method is used by the owner for updating the entity's contents.
    ///
    Status		Entity::Update(PrivateKey&		owner,
				       Address&			data)
    {
      // call the other Update() method by using the owner's key
      // as a normal user key.
      if (this->Update(owner, data, 0) == StatusError)
	escape("unable to update the contents");

      // then, overwrite the writer part which has been set as
      // being the user with index 0.
      //
      // let's rewrite this part so that the owner becomes the
      // writer.
      this->writer.mode = Entity::ModeOwner;
      this->writer.index = 0;

      leave();
    }

    ///
    /// this method modifies the data by recording which user is
    /// the writer through the given index.
    ///
    /// this method updates the attributes before building an
    /// archive and signing it.
    ///
    Status		Entity::Update(PrivateKey&		user,
				       Address&			data,
				       Natural32		index)
    {
      Archive		archive;

      // set the data address.
      this->contents.data = data;

      // increase the contents version.
      this->contents.version += 1;

      // create an archive containing both the address and the version so
      // that this archive can be signed.
      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      // record the elements: data and version.
      if (archive.Serialize(this->contents.data,
			    this->contents.version) == StatusError)
	escape("unable to serialize the contents' address and version");

      // sign the archive with the user key.
      if (user.Sign(archive, this->contents.signature) == StatusError)
	escape("unable to sign the contents' archive");

      // set the writer.
      this->writer.mode = Entity::ModeUser;
      this->writer.index = index;

      leave();
    }

    ///
    /// this method enables the owner to manage the entity's meta data.
    ///
    /// the method first update the attributes before signing a
    /// temporarily created archive.
    ///
    Status		Entity::Manage(PrivateKey&		owner,
				       Access::Rights		rights,
				       Address&			access)
    {
      Archive		archive;

      // set the owner rights
      this->meta.rights = rights;

      // set the users access address.
      this->meta.access = access;

      // increase the meta version.
      this->meta.version += 1;

      // create an archive.
      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      // serialize the elements: type, rights, address and version.
      if (archive.Serialize((Byte&)this->meta.type,
			    (Byte&)this->meta.rights,
			    this->meta.access,
			    this->meta.version) == StatusError)
	escape("unable to serialize the meta elements");

      // sign the meta data.
      if (owner.Sign(archive, this->meta.signature) == StatusError)
	escape("unable to sign the meta archive");

      leave();
    }

    ///
    /// this method verifies that the entity has been properly created
    /// i.e that every signature has been produced by allowed users.
    ///
    /// the method (i) calls the parent class for validation (ii)
    /// verify the owner part's signature (iii) verify the contents
    /// signature by retrieving the writer public key---note that
    /// the writer can be the owner according to the writer mode---
    /// (iv) verify the meta part's signature.
    ///
    Status		Entity::Validate(Address&		address)
    {
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
	// XXX
      }

      // (iv)
      {
	Archive		archive;

	// create an archive.
	if (archive.Create() == StatusError)
	  flee("unable to create an archive");

	// serialize the meta part ... without the signature.
	if (archive.Serialize((Byte&)this->meta.type,
			      (Byte&)this->meta.rights,
			      this->meta.access,
			      this->meta.version) == StatusError)
	  flee("unable to serialize the meta part");

	// verify the meta signature.
	if (this->owner.K.Verify(this->meta.signature,
				 archive) == StatusError)
	  false();
      }

      true();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this method initializes the object.
    ///
    Status		Entity::New(Entity&			entity)
    {
      entity.contents.version = 0;

      entity.writer.mode = Entity::ModeUnknown;
      entity.writer.index = 0;

      entity.meta.type = Entity::TypeUnknown;
      entity.meta.rights = Access::RightNone;
      entity.meta.version = 0;

      leave();
    }

    ///
    /// this method reinitializes the object.
    ///
    Status		Entity::Delete(Entity&)
    {
      leave();
    }

    ///
    /// assign the entity.
    ///
    Entity&		Entity::operator=(const Entity&		element)
    {
      // self-check.
      if (this == &element)
	return (*this);

      // call the parent class.
      PublicKeyBlock::operator=(element);

      // reinitialize the object.
      if ((Entity::Delete(*this) == StatusError) ||
	  (Entity::New(*this) == StatusError))
	yield("unable to reinitialize the object", *this);

      // assign the attributes.
      this->owner.K = element.owner.K;
      this->owner.signature = element.owner.signature;

      this->contents.data = element.contents.data;
      this->contents.version = element.contents.version;
      this->contents.signature = element.contents.signature;

      this->writer.mode = element.writer.mode;
      this->writer.index = element.writer.index;

      this->meta.type = element.meta.type;
      this->meta.rights = element.meta.rights;
      this->meta.access = element.meta.access;
      this->meta.version = element.meta.version;
      this->meta.signature = element.meta.signature;

      return (*this);
    }

    ///
    /// this method checks if two objects match.
    ///
    Boolean		Entity::operator==(const Entity&		element)
    {
      // call the parent class.
      if (PublicKeyBlock::operator==(element) == false)
	false();

      // compare the owner part.
      if ((this->owner.K != element.owner.K) ||
	  (this->owner.signature != element.owner.signature))
	false();

      // compare the contents part.
      if ((this->contents.data != element.contents.data) ||
	  (this->contents.version != element.contents.version) ||
	  (this->contents.signature != element.contents.signature))
	false();

      // compare the writer part.
      if ((this->writer.mode != element.writer.mode) ||
	  (this->writer.index != element.writer.index))
	false();

      // compare the meta part.
      if ((this->meta.type != element.meta.type) ||
	  (this->meta.rights != element.meta.rights) ||
	  (this->meta.access != element.meta.access) ||
	  (this->meta.version != element.meta.version) ||
	  (this->meta.signature != element.meta.signature))
	false();

      true();
    }

    ///
    /// this method checks if two objects dis-match.
    ///
    Boolean		Entity::operator!=(const Entity&		element)
    {
      return (!(*this == element));
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an entity object.
    ///
    Status		Entity::Dump(Natural32			margin)
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');

      std::cout << alignment << "[Entity]" << std::endl;

      // dump the parent class.
      if (PublicKeyBlock::Dump(margin + 2) == StatusError)
	escape("unable to dump the underlying public key block");

      // dump the owner part.
      std::cout << alignment << shift << "[Owner]" << std::endl;

      if (this->owner.K.Dump(4) == StatusError)
	escape("unable to dump the owner's public key");

      if (this->owner.signature.Dump(4) == StatusError)
	escape("unable to dump the owner's signature");

      // dump the contents part.
      std::cout << alignment << shift << "[Contents]" << std::endl;

      if (this->contents.data.Dump(4) == StatusError)
	escape("unable to dump the contents' data");

      std::cout << alignment << shift << shift << "[Version] " << this->contents.version << std::endl;

      if (this->contents.signature.Dump(4) == StatusError)
	escape("unable to dump the contents' signature");

      // dump the writer part.
      std::cout << alignment << shift << "[Writer]" << std::endl;

      std::cout << alignment << shift << shift << "[Mode] " << this->writer.mode << std::endl;
      std::cout << alignment << shift << shift << "[Index] " << this->writer.index << std::endl;

      // dump the meta part.
      std::cout << alignment << shift << "[Meta]" << std::endl;

      std::cout << alignment << shift << shift << "[Type] " << this->meta.type << std::endl;
      std::cout << alignment << shift << shift << "[Rights] " << this->meta.rights << std::endl;

      if (this->meta.access.Dump(4) == StatusError)
	escape("unable to dump the meta access address");

      std::cout << alignment << shift << shift << "[Version] " << this->meta.version << std::endl;

      if (this->meta.signature.Dump(4) == StatusError)
	escape("unable to dump the meta signature");

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the entity object.
    ///
    Status		Entity::Serialize(Archive&		archive) const
    {
      Archive		ar;

      // call the parent class.
      if (PublicKeyBlock::Serialize(archive) == StatusError)
	escape("unable to serialize the underlying PKB");

      // prepare the object archive.
      if (ar.Create() == StatusError)
	escape("unable to prepare the object archive");

      // serialize the class name.
      if (ar.Serialize(Entity::Class) == StatusError)
	escape("unable to serialize the class name");

      // serialize the owner part.
      if (ar.Serialize(this->owner.K,
		       this->owner.signature) == StatusError)
	escape("unable to serialize the owner part");

      // serialize the contents part.
      if (ar.Serialize(this->contents.data,
		       this->contents.version,
		       this->contents.signature) == StatusError)
	escape("unable to serialize the contents part");

      // serialize the writer part.
      if (ar.Serialize((Byte&)this->writer.mode,
		       this->writer.index) == StatusError)
	escape("unable to serialize the writer part");

      // serialize the meta part.
      if (ar.Serialize((Byte&)this->meta.type,
		       (Byte&)this->meta.rights,
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
    /// this method extracts the entity object.
    ///
    Status		Entity::Extract(Archive&		archive)
    {
      Archive		ar;
      String		name;

      // call the parent class.
      if (PublicKeyBlock::Extract(archive) == StatusError)
	escape("unable to extract the underyling PKB");

      // extract the entity archive object.
      if (archive.Extract(ar) == StatusError)
	escape("unable to extract the entity archive object");

      // extract the name.
      if (ar.Extract(name) == StatusError)
	escape("unable to extract the class name");

      // extract the owner part.
      if (ar.Extract(this->owner.K,
		     this->owner.signature) == StatusError)
	escape("unable to extract the owner part");

      // extract the contents part.
      if (ar.Extract(this->contents.data,
		     this->contents.version,
		     this->contents.signature) == StatusError)
	escape("unable to extract the contents part");

      // extract the writer part.
      if (ar.Extract((Byte&)this->writer.mode,
		     this->writer.index) == StatusError)
	escape("unable to extract the writer part");

      // extract the meta part.
      if (ar.Extract((Byte&)this->meta.type,
		     (Byte&)this->meta.rights,
		     this->meta.access,
		     this->meta.version,
		     this->meta.signature) == StatusError)
	escape("unable to extract the meta part");

      // check the name.
      if (Entity::Class != name)
	escape("wrong class name in the extract object");

      leave();
    }

  }
}
