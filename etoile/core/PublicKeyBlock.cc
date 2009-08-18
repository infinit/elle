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
// updated       julien quintard   [fri aug 14 21:04:09 2009]
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
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a PKB by generating a keypair.
    ///
    Status		PublicKeyBlock::Create(const KeyPair&	pair)
    {
      // copy the public key.
      this->K = pair.K;

      leave();
    }

    ///
    /// this method computes the block's address.
    ///
    Status		PublicKeyBlock::Self(hole::Address&	address)
      const
    {
      // compute the address.
      if (address.Create(this->K) == StatusError)
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

      // retrieve the address of this object.
      if (this->Self(self) == StatusError)
	flee("unable to retrieve the block's address");

      // compare the address with the given one.
      if (self != address)
	flee("the given address does not correspond to this block");

      true();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an block object.
    ///
    Status		PublicKeyBlock::Dump(Natural32	margin) const
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');

      std::cout << alignment << "[PublicKeyBlock]" << std::endl;

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
      // extract the public key.
      if (archive.Extract(this->K) == StatusError)
	escape("unable to extract the public key");

      leave();
    }

  }
}
