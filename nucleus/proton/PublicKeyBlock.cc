//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [tue feb 17 18:09:00 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/proton/PublicKeyBlock.hh>
#include <nucleus/proton/Family.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    PublicKeyBlock::PublicKeyBlock():
      MutableBlock()
    {
    }

    ///
    /// specific constructor.
    ///
    PublicKeyBlock::PublicKeyBlock(const neutron::Component     component):
      MutableBlock(FamilyPublicKeyBlock, component)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a PKB based on the given public key.
    ///
    elle::Status        PublicKeyBlock::Create(const elle::PublicKey& K)
    {
      // copy the public key.
      this->K = K;

      return elle::StatusOk;
    }

    ///
    /// this method computes the block's address.
    ///
    elle::Status        PublicKeyBlock::Bind(Address&           address)
      const
    {
      // compute the address.
      if (address.Create(this->family, this->component,
                         this->network,
                         static_cast<elle::Natural8>(this->family),
                         static_cast<elle::Natural8>(this->component),
                         this->K) == elle::StatusError)
        escape("unable to compute the PKB's address");

      return elle::StatusOk;
    }

    ///
    /// this method verifies the block's validity.
    ///
    elle::Status        PublicKeyBlock::Validate(const Address& address)
      const
    {
      Address           self;

      //
      // make sure the address has not be tampered and correspond to the
      // hash of the public key.
      //

      // compute the address.
      if (self.Create(this->family, this->component,
                      this->network,
                      static_cast<elle::Natural8>(this->family),
                      static_cast<elle::Natural8>(this->component),
                      this->K) == elle::StatusError)
        escape("unable to compute the PKB's address");

      // verify with the recorded address.
      if (address != self)
        escape("the address does not correspond to the block's public key");

      //
      // at this point the node knows that the recorded address corresponds
      // to the recorded public key and identifies the block requested.
      //

      return elle::StatusTrue;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the object.
    ///
    embed(PublicKeyBlock, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an block object.
    ///
    elle::Status        PublicKeyBlock::Dump(const
                                               elle::Natural32  margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[PublicKeyBlock]" << std::endl;

      // dump the parent class.
      if (MutableBlock::Dump(margin + 2) == elle::StatusError)
        escape("unable to dump the underlying block");

      // dump the PKB's public key.
      std::cout << alignment << elle::Dumpable::Shift << "[K]" << std::endl;

      if (this->K.Dump(margin + 4) == elle::StatusError)
        escape("unable to dump the public key");

      return elle::StatusOk;
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the block object.
    ///
    elle::Status        PublicKeyBlock::Serialize(elle::Archive& archive) const
    {
      // serialize the parent class.
      if (MutableBlock::Serialize(archive) == elle::StatusError)
        escape("unable to serialize the underlying block");

      // serialize the public key.
      if (archive.Serialize(this->K) == elle::StatusError)
        escape("unable to serialize the public key");

      return elle::StatusOk;
    }

    ///
    /// this method extracts the block object.
    ///
    elle::Status        PublicKeyBlock::Extract(elle::Archive&  archive)
    {
      // extract the parent class.
      if (MutableBlock::Extract(archive) == elle::StatusError)
        escape("unable to extract the underlying block");

      // check the family.
      if (this->family != FamilyPublicKeyBlock)
        escape("invalid family");

      // extract the public key.
      if (archive.Extract(this->K) == elle::StatusError)
        escape("unable to extract the public key");

      return elle::StatusOk;
    }

  }
}
