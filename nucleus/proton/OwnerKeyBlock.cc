#include <nucleus/proton/OwnerKeyBlock.hh>
#include <nucleus/proton/Family.hh>
#include <nucleus/proton/Network.hh>
#include <nucleus/proton/Address.hh>

#include <elle/cryptography/KeyPair.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- construction ----------------------------------------------------
//

    OwnerKeyBlock::OwnerKeyBlock():
      MutableBlock(),

      _owner_subject(nullptr)
    {
    }

    OwnerKeyBlock::OwnerKeyBlock(
        Network const& network,
        neutron::Component const component,
        elle::cryptography::PublicKey const& creator_K):
      OwnerKeyBlock(network, component, creator_K,
                    elle::cryptography::KeyPair::generate())
    {
    }

    OwnerKeyBlock::OwnerKeyBlock(
        Network const& network,
        neutron::Component const component,
        elle::cryptography::PublicKey const& creator_K,
        elle::cryptography::KeyPair const& block_pair):
      MutableBlock(network, FamilyOwnerKeyBlock, component, creator_K),

      _block_K(block_pair.K),
      _owner_K(creator_K),
      _owner_signature(block_pair.k.sign(this->_owner_K)),
      _owner_subject(nullptr)
    {
    }

    OwnerKeyBlock::~OwnerKeyBlock()
    {
      delete this->_owner_subject;
    }

//
// ---------- block -----------------------------------------------------------
//

    Address
    OwnerKeyBlock::bind() const
    {
      Address address(this->network(), this->family(), this->component(),
                      this->_block_K);

      return (address);
    }

    void
    OwnerKeyBlock::validate(Address const& address) const
    {
      if ((this->network() != address.network()) ||
          (this->family() != address.family()) ||
          (this->component() != address.component()))
        throw Exception(
          elle::sprint("the address %s does not seem to represent the given "
                       "block", address));

      //
      // make sure the address has not be tampered and correspond to the
      // hash of the public key.
      //
      Address self(this->network(), this->family(), this->component(),
                   this->_block_K);

      // verify with the recorded address.
      if (address != self)
        throw Exception("the address does not correspond to the block's "
                        "public key");

      // verify the owner's key signature with the block's public key.
      if (this->_block_K.Verify(this->_owner_signature,
                                this->_owner_K) == elle::Status::Error)
        throw Exception("unable to verify the owner's signature");
    }

    neutron::Subject const&
    OwnerKeyBlock::owner_subject()
    {
      // Create the subject corresponding to the block owner, if necessary.
      // Note that this subject will never be serialized but is used to ease
      // the process of access control since most method require a subject.
      if (this->_owner_subject == nullptr)
        this->_owner_subject = new neutron::Subject(this->_owner_K);

      assert(this->_owner_subject != nullptr);

      return (*this->_owner_subject);
    }

//
// ---------- dumpable --------------------------------------------------------
//

    elle::Status
    OwnerKeyBlock::Dump(const elle::Natural32  margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[OwnerKeyBlock]" << std::endl;

      // dump the parent class.
      if (MutableBlock::Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the underlying block");

      // dump the OKB's public key.
      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Block K]" << std::endl;

      if (this->_block_K.Dump(margin + 4) == elle::Status::Error)
        escape("unable to dump the public key");

      // dump the owner part.
      std::cout << alignment << elle::io::Dumpable::Shift << "[Owner]"
                << std::endl;

      std::cout << alignment << elle::io::Dumpable::Shift
                << elle::io::Dumpable::Shift
                << "[K]" << std::endl;

      if (this->_owner_K.Dump(margin + 6) == elle::Status::Error)
        escape("unable to dump the owner's public key");

      std::cout << alignment << elle::io::Dumpable::Shift
                << elle::io::Dumpable::Shift
                << "[Signature]" << std::endl;

      if (this->_owner_signature.Dump(margin + 6) == elle::Status::Error)
        escape("unable to dump the owner's signature");

      if (this->_owner_subject != nullptr)
        if (this->_owner_subject->Dump(margin + 6) == elle::Status::Error)
          escape("unable to dump the subject");

      return elle::Status::Ok;
    }

//
// ---------- printable -------------------------------------------------------
//

    void
    OwnerKeyBlock::print(std::ostream& stream) const
    {
      stream << "owner key block{"
             << this->_block_K
             << ", "
             << this->_owner_K
             << "}";
    }

  }
}
