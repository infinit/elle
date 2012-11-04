#include <nucleus/proton/ImprintBlock.hh>
#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Family.hh>
#include <nucleus/neutron/Subject.hh>
#include <nucleus/Exception.hh>

#include <elle/cryptography/PublicKey.hh>

#include <elle/idiom/Open.hh>

ELLE_LOG_COMPONENT("infinit.nucleus.proton.ImprintBlock");

namespace nucleus
{
  namespace proton
  {

    /*-------------.
    | Construction |
    `-------------*/

    ImprintBlock::ImprintBlock():
      MutableBlock(),

      _owner_subject(nullptr)
    {
    }

    ImprintBlock::ImprintBlock(Network const& network,
                               neutron::Component const component,
                               elle::cryptography::PublicKey const& creator_K):
      MutableBlock(network, Family::imprint_block, component, creator_K),

      _owner_K(creator_K),
      _owner_subject(nullptr)
    {
    }

    ImprintBlock::~ImprintBlock()
    {
      delete this->_owner_subject;
    }

    /*--------.
    | Methods |
    `--------*/

    neutron::Subject const&
    ImprintBlock::owner_subject()
    {
      // Create the subject corresponding to the block owner, if necessary.
      // Note that this subject will never be serialized but is used to ease
      // the process of access control since most method require a subject.
      if (this->_owner_subject == nullptr)
        this->_owner_subject = new neutron::Subject(this->_owner_K);

      assert(this->_owner_subject != nullptr);

      return (*this->_owner_subject);
    }

    /*-----------.
    | Interfaces |
    `-----------*/

    Address
    ImprintBlock::bind() const
    {
      ELLE_TRACE_METHOD("");

      /// The computation of an Imprint block's address consists in hashing
      /// the creation timestamp and salt along with the owner's public key so as
      /// to ensure the uniqueness of the address.
      Address address(this->network(), this->family(), this->component(),
                      this->creation_timestamp(), this->salt(), this->_owner_K);

      return (address);
    }

    void
    ImprintBlock::validate(Address const& address) const
    {
      ELLE_TRACE_METHOD(address);

      if ((this->network() != address.network()) ||
          (this->family() != address.family()) ||
          (this->component() != address.component()))
        throw Exception(
          elle::sprint("the address %s does not seem to represent the given "
                       "block", address));

      // Make sure the block has not be tampered and correspond to the
      // given original address. In order to do that, the address is
      // recomputed by hashing the tuple (network, family, component,
      // timestamp, salt, owner_K).
      Address self(this->network(), this->family(), this->component(),
                   this->creation_timestamp(), this->salt(), this->_owner_K);

      // Finally, compare the recomputed address with the theoretical address
      // of the block.
      if (address != self)
        throw Exception("the address does not correspond to the block's "
                        "public key");
    }

    void
    ImprintBlock::print(std::ostream& stream) const
    {
      stream << "imprint block{"
             << this->_owner_K
             << "}";
    }

    elle::Status
    ImprintBlock::Dump(const elle::Natural32 margin) const
    {
      elle::String alignment(margin, ' ');

      std::cout << alignment << "[ImprintBlock]" << std::endl;

      if (MutableBlock::Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the underlying block");

      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Owner]" << std::endl;

      if (this->_owner_K.Dump(margin + 4) == elle::Status::Error)
        escape("unable to dump the owner's public key");

      if (this->_owner_subject != nullptr)
        {
          if (this->_owner_subject->Dump(margin + 4) == elle::Status::Error)
            escape("unable to dump the subject");
        }
      else
        {
          std::cout << alignment << elle::io::Dumpable::Shift
                    << elle::io::Dumpable::Shift
                    << "[Subject] " << elle::none << std::endl;
        }

      return elle::Status::Ok;
    }

  }
}
