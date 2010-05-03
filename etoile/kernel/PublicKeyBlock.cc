//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/PublicKeyBlock.cc
//
// created       julien quintard   [tue feb 17 18:09:00 2009]
// updated       julien quintard   [mon may  3 20:52:59 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/kernel/PublicKeyBlock.hh>

namespace etoile
{
  namespace kernel
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// this constructor set the family and kind of the underlying block.
    ///
    PublicKeyBlock::PublicKeyBlock(const hole::Component&	component):
      hole::Block(hole::FamilyPublicKeyBlock, component)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a PKB based on the given public key.
    ///
    elle::Status	PublicKeyBlock::Create(const elle::PublicKey&	K)
    {
      enter();

      // copy the public key.
      this->K = K;

      leave();
    }

    ///
    /// this method computes the block's address.
    ///
    elle::Status	PublicKeyBlock::Bind()
    {
      enter();

      // compute the address.
      if (this->address.Create(this->family, this->K) == elle::StatusError)
	escape("unable to compute the PKB's address");

      leave();
    }

    ///
    /// this method verifies the block's validity.
    ///
    elle::Status	PublicKeyBlock::Validate(const hole::Address& address)
      const
    {
      hole::Address	self;

      enter();

      //
      // make sure the address has not be tampered and correspond to the
      // hash of the public key.
      //

      // compute the address.
      if (self.Create(this->family, this->K) == elle::StatusError)
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
    elle::Status	PublicKeyBlock::Dump(const
					       elle::Natural32	margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[PublicKeyBlock]" << std::endl;

      // dump the parent class.
      if (hole::Block::Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the underlying block");

      // dump the PKB's public key.
      std::cout << alignment << elle::Dumpable::Shift << "[K]" << std::endl;
      if (this->K.Dump(margin + 4) == elle::StatusError)
	escape("unable to dump the public key");

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the block object.
    ///
    elle::Status	PublicKeyBlock::Serialize(elle::Archive& archive) const
    {
      enter();

      // serialize the parent class.
      if (hole::Block::Serialize(archive) == elle::StatusError)
	escape("unable to serialize the underlying block");

      // serialize the public key.
      if (archive.Serialize(this->K) == elle::StatusError)
	escape("unable to serialize the public key");

      leave();
    }

    ///
    /// this method extracts the block object.
    ///
    elle::Status	PublicKeyBlock::Extract(elle::Archive&	archive)
    {
      enter();

      // extract the parent class.
      if (hole::Block::Extract(archive) == elle::StatusError)
	escape("unable to extract the underlying block");

      // extract the public key.
      if (archive.Extract(this->K) == elle::StatusError)
	escape("unable to extract the public key");

      leave();
    }

  }
}
