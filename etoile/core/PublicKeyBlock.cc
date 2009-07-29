//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/PublicKeyBlock.cc
//
// created       julien quintard   [tue feb 17 18:09:00 2009]
// updated       julien quintard   [wed jul 29 17:28:41 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/core/PublicKeyBlock.hh>

namespace etoile
{
  namespace core
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// the class name.
    ///
    const String		PublicKeyBlock::Class = "PublicKeyBlock";

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// this method initializes the object.
    ///
    PublicKeyBlock::PublicKeyBlock()
    {
      this->k = NULL;
    }

    ///
    /// this method releases and reinitializes.
    ///
    PublicKeyBlock::~PublicKeyBlock()
    {
      // release the private key.
      if (this->k != NULL)
	delete this->k;
    }

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
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an block object.
    ///
    Status		PublicKeyBlock::Dump(Natural32	margin)
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');

      std::cout << alignment << "[PublicKeyBlock]" << std::endl;

      // dump the parent class.
      if (Block::Dump(margin + 2) == StatusError)
	escape("unable to dump the block");

      // dump the PKB's public key.
      std::cout << alignment << shift << "[K]" << std::endl;
      if (this->K.Dump(margin + 4) == StatusError)
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
