//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/infinit/components/PublicKeyBlock.cc
//
// created       julien quintard   [tue feb 17 18:09:00 2009]
// updated       julien quintard   [wed mar 11 16:22:16 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/components/PublicKeyBlock.hh>

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
    const String		PublicKeyBlock::Class = "PublicKeyBlock";

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a PKB by generating a keypair.
    ///
    Status		PublicKeyBlock::Create()
    {
      KeyPair		pair;

      // generate the block keypair.
      if (pair.Generate() == StatusError)
	escape("unable to create the block's keypair");

      // copy the public key.
      this->K = pair.K;

      // create and copy the private key.
      this->k = new PrivateKey;
      *this->k = pair.k;

      leave();
    }

    ///
    /// this method seals the object by computing its address.
    ///
    Status		PublicKeyBlock::Seal()
    {
      // compute the address.
      if (this->address.Create(this->K) == StatusError)
	escape("unable to compute the CHB's address");

      leave();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this method initializes the object.
    ///
    Status		PublicKeyBlock::New(PublicKeyBlock&		pkb)
    {
      pkb.k = NULL;

      leave();
    }

    ///
    /// this method releases and reinitializes.
    ///
    Status		PublicKeyBlock::Delete(PublicKeyBlock&		pkb)
    {
      // release the private key.
      if (pkb.k != NULL)
	delete pkb.k;

      leave();
    }

    ///
    /// assign the public key block.
    ///
    PublicKeyBlock&	PublicKeyBlock::operator=(const PublicKeyBlock&	element)
    {
      // self-check.
      if (this == &element)
	return (*this);

      // call the parent class.
      Block::operator=(element);

      // reinitialize the object.
      if ((PublicKeyBlock::Delete(*this) == StatusError) ||
	  (PublicKeyBlock::New(*this) == StatusError))
	yield("unable to reinitialize the object", *this);

      // assign the public key.
      this->K = element.K;

      // assign the private key, if present.
      if (element.k != NULL)
	{
	  this->k = new PrivateKey;

	  *this->k = *element.k;
	}

      return (*this);
    }

    ///
    /// this method checks if two objects match.
    ///
    /// note that two objects cannot be compared if they have not been sealed.
    /// indeed, this method starts by calling the parent Block class that
    /// compares the addresses. since these addresses are computed when
    /// the blocks are sealed, the process will failed if the two blocks
    /// have not been sealed.
    ///
    /// note that the public key is not compared in this method since
    /// the addresses are compared and those addresses are hashes of the
    /// public keys.
    ///
    Boolean		PublicKeyBlock::operator==(const PublicKeyBlock& element)
    {
      // call the parent class.
      return (Block::operator==(element));
    }

    ///
    /// this method checks if two objects dis-match.
    ///
    Boolean		PublicKeyBlock::operator!=(const PublicKeyBlock& element)
    {
      return (!(*this == element));
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an block object.
    ///
    Status		PublicKeyBlock::Dump(Natural32	margin)
    {
      String		alignment(margin, ' ');

      std::cout << alignment << "[PublicKeyBlock]" << std::endl;

      if (Block::Dump(margin + 2) == StatusError)
	escape("unable to dump the block");

      if (this->K.Dump(margin + 2) == StatusError)
	escape("unable to dump the public key");

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the block object.
    ///
    Status		PublicKeyBlock::Serialize(Archive&	archive) const
    {
      Archive		ar;

      // call the parent class.
      if (Block::Serialize(archive) == StatusError)
	escape("unable to serialize the block");

      // prepare the object archive.
      if (ar.Create() == StatusError)
	escape("unable to prepare the object archive");

      // serialize the class elements.
      if (ar.Serialize(PublicKeyBlock::Class, this->K) == StatusError)
	escape("unable to serialize the class elements");

      // record the object archive into the given archive.
      if (archive.Serialize(ar) == StatusError)
	escape("unable to serialize the object archive");

      leave();
    }

    ///
    /// this method extracts the block object.
    ///
    Status		PublicKeyBlock::Extract(Archive&	archive)
    {
      Archive		ar;
      String		name;

      // call the parent class.
      if (Block::Extract(archive) == StatusError)
	escape("unable to extract the block");

      // extract the block archive object.
      if (archive.Extract(ar) == StatusError)
	escape("unable to extract the block archive object");

      // extract the elements.
      if (ar.Extract(name, this->K) == StatusError)
	escape("unable to extract the class elements");

      // check the name.
      if (PublicKeyBlock::Class != name)
	escape("wrong class name in the extract object");

      leave();
    }

  }
}
