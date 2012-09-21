#include <nucleus/proton/ImprintBlock.hh>
#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Family.hh>
#include <nucleus/neutron/Subject.hh>
#include <nucleus/Exception.hh>

#include <elle/cryptography/Random.hh>
#include <elle/cryptography/PublicKey.hh>

#include <elle/idiom/Open.hh>

ELLE_LOG_COMPONENT("infinit.nucleus.proton.ImprintBlock");

namespace nucleus
{
  namespace proton
  {

//
// ---------- construction ----------------------------------------------------
//

    ImprintBlock::ImprintBlock():
      MutableBlock(),

      _owner_subject(nullptr)
    {
    }

    ImprintBlock::ImprintBlock(Network const& network,
                               neutron::Component const component,
                               elle::cryptography::PublicKey const& creator_K):
      MutableBlock(network, FamilyImprintBlock, component, creator_K)
    {
      // Compute the creation timestamp.
      if (this->_creation_stamp.Current() == elle::Status::Error)
        throw Exception("unable to retrieve the current time");

      // Generate a random number for the salt.
      if (elle::cryptography::Random::Generate(this->_salt) == elle::Status::Error)
        throw Exception("unable to generate the seed");

      // Set the owner public key.
      this->_owner_K = creator_K; // XXX[?]
      this->_owner_subject = nullptr;
    }

    ImprintBlock::~ImprintBlock()
    {
      delete this->_owner_subject;
    }

//
// ---------- methods ---------------------------------------------------------
//

    Address
    ImprintBlock::bind() const
    {
      Address address;

      ELLE_TRACE_SCOPE("[%p] Bind()", this);

      // Compute the address of the block by hashing the following
      // elements: network, family, component, stamp, salt and owner_K..
      if (address.Create(this->network(), this->family(), this->component(),
                         this->_creation_stamp, this->_salt, this->_owner_K) ==
          elle::Status::Error)
        throw Exception("unable to compute the imprint address");

      return (address);
    }

    void
    ImprintBlock::validate(Address const& address) const
    {
      Address self;

      ELLE_TRACE_SCOPE("Validate() this(%p)", this);

      // Make sure the block has not be tampered and correspond to the
      // given original address. In order to do that, the address is
      // recomputed by hashing the tuple (network, family, component,
      // stamp, salt, owner_K).
      if (self.Create(this->network(), this->family(), this->component(),
                      this->_creation_stamp, this->_salt, this->_owner_K) ==
          elle::Status::Error)
        throw Exception("unable to compute the imprint address");

      // Finally, compare the recomputed address with the theoretical address
      // of the block.
      if (address != self)
        throw Exception("the address does not correspond to the block's public key");
    }

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

//
// ---------- dumpable --------------------------------------------------------
//

    elle::Status
    ImprintBlock::Dump(const elle::Natural32 margin) const
    {
      elle::String alignment(margin, ' ');

      std::cout << alignment << "[ImprintBlock]" << std::endl;

      if (MutableBlock::Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the underlying block");

      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Creation Stamp]" << std::endl;

      if (this->_creation_stamp.Dump(margin + 4) == elle::Status::Error)
        escape("unable to dump the stamp");

      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Salt] " << this->_salt << std::endl;

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
