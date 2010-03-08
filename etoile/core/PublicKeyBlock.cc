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
// updated       julien quintard   [wed mar  3 16:17:04 2010]
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
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// this constructor set the family and kind of the underlying block.
    ///
    PublicKeyBlock::PublicKeyBlock():
      hole::Block(hole::FamilyPublicKeyBlock)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a PKB by generating a keypair.
    ///
    Status		PublicKeyBlock::Create(const KeyPair&	pair)
    {
      enter();

      // copy the public key.
      this->K = pair.K;

      leave();
    }

    ///
    /// this method computes the block's address.
    ///
    Status		PublicKeyBlock::Bind()
    {
      enter();

      // compute the address.
      if (this->address.Create(this->family, this->K) == StatusError)
	escape("unable to compute the PKB's address");

      leave();
    }

    ///
    /// this method verifies the block's validity.
    ///
    Status		PublicKeyBlock::Validate(const hole::Address& address)
      const
    {
      hole::Address	self;

      enter();

      //
      // make sure the address has not be tampered and correspond to the
      // hash of the public key.
      //

      // compute the address.
      if (self.Create(this->family, this->K) == StatusError)
	escape("unable to compute the PKB's address");

      // verify with the recorded address.
      if (this->address != self)
	escape("the address does not correspond to the block's public key");

      //
      // at this point the node knows that the recorded address corresponds
      // to the recorded public key and identifies the block requested.
      //

      true();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an block object.
    ///
    Status		PublicKeyBlock::Dump(const Natural32	margin) const
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');

      enter();

      std::cout << alignment << "[PublicKeyBlock]" << std::endl;

      // dump the parent class.
      if (hole::Block::Dump(margin + 2) == StatusError)
	escape("unable to dump the underlying block");

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
      enter();

      // serialize the parent class.
      if (hole::Block::Serialize(archive) == StatusError)
	escape("unable to serialize the underlying block");

      // serialize the public key.
      if (archive.Serialize(this->K) == StatusError)
	escape("unable to serialize the public key");

      leave();
    }

    ///
    /// this method extracts the block object.
    ///
    Status		PublicKeyBlock::Extract(Archive&	archive)
    {
      enter();

      // extract the parent class.
      if (hole::Block::Extract(archive) == StatusError)
	escape("unable to extract the underlying block");

      // extract the public key.
      if (archive.Extract(this->K) == StatusError)
	escape("unable to extract the public key");

      leave();
    }

  }
}
