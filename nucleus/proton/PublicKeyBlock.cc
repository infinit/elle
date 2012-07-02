
#include <elle/cryptography/PublicKey.hh>
#include <nucleus/proton/Block.hh>

#include <nucleus/proton/Network.hh>
#include <nucleus/proton/PublicKeyBlock.hh>
#include <nucleus/proton/Family.hh>

#include <elle/idiom/Open.hh>

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
    elle::Status        PublicKeyBlock::Create(elle::cryptography::PublicKey const& K)
    {
      // copy the public key.
      this->K = K;

      return elle::Status::Ok;
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
                         this->family,
                         this->component,
                         this->K) == elle::Status::Error)
        escape("unable to compute the PKB's address");

      return elle::Status::Ok;
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
                      this->family,
                      this->component,
                      this->K) == elle::Status::Error)
        escape("unable to compute the PKB's address");

      // verify with the recorded address.
      if (address != self)
        escape("the address does not correspond to the block's public key");

      //
      // at this point the node knows that the recorded address corresponds
      // to the recorded public key and identifies the block requested.
      //

      return elle::Status::Ok;
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
      if (MutableBlock::Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the underlying block");

      // dump the PKB's public key.
      std::cout << alignment << elle::io::Dumpable::Shift << "[K]" << std::endl;

      if (this->K.Dump(margin + 4) == elle::Status::Error)
        escape("unable to dump the public key");

      return elle::Status::Ok;
    }

  }
}
