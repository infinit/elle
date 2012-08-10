#include <nucleus/proton/ImprintBlock.hh>
#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Family.hh>
#include <nucleus/neutron/Subject.hh>
#include <nucleus/Exception.hh>

#include <elle/cryptography/Random.hh>
#include <elle/cryptography/PublicKey.hh>

#include <elle/idiom/Open.hh>

ELLE_LOG_TRACE_COMPONENT("infinit.nucleus.proton.ImprintBlock");

namespace nucleus
{
  namespace proton
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ImprintBlock::ImprintBlock():
      MutableBlock()
    {
      this->_owner.subject = nullptr;
    }

    ImprintBlock::ImprintBlock(neutron::Component const component):
      MutableBlock(FamilyImprintBlock, component)
    {
      this->_owner.subject = nullptr;
    }

    ImprintBlock::ImprintBlock(neutron::Component const component,
                               elle::cryptography::PublicKey const& owner):
      MutableBlock(FamilyImprintBlock, component)
    {
      this->_owner.subject = nullptr;

      if (this->Create(owner) == elle::Status::Error)
        throw Exception("unable to create the imprint block");
        // XXX[no longer necessary in the future]
    }

    ImprintBlock::~ImprintBlock()
    {
      delete this->_owner.subject;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the object.
    ///
    embed(ImprintBlock, _());

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates an imprint based on the given owner's public key.
    ///
    elle::Status
    ImprintBlock::Create(elle::cryptography::PublicKey const& owner)
    {
      ELLE_LOG_TRACE_SCOPE("Create() this(%p)", this);

      // retrieve the current time.
      if (this->_stamp.Current() == elle::Status::Error)
        escape("unable to retrieve the current time");

      // generate a random number.
      if (elle::cryptography::Random::Generate(this->_salt) == elle::Status::Error)
        escape("unable to generate the seed");

      // set the owner public key.
      assert(owner.key() != nullptr);
      this->_owner.K = owner;

      return elle::Status::Ok;
    }

    ///
    /// this method computes the block's address.
    ///
    elle::Status
    ImprintBlock::Bind(Address& address) const
    {
      ELLE_LOG_TRACE_SCOPE("Bind() this(%p)", this);

      assert(this->_owner.K.key() != nullptr);

      // compute the address.
      if (address.Create(this->family, this->component,
                         this->network,
                         this->family,
                         this->component,
                         this->_stamp, this->_salt, this->_owner.K) ==
          elle::Status::Error)
        escape("unable to compute the imprint address");

      return elle::Status::Ok;
    }

    ///
    /// this method verifies the block's validity.
    ///
    elle::Status
    ImprintBlock::Validate(Address const& address) const
    {
      Address self;

      ELLE_LOG_TRACE_SCOPE("Validate() this(%p)", this);

      assert(this->_owner.K.key() != nullptr);

      //
      // make sure the address has not be tampered and correspond to the
      // hash of the tuple (stamp, salt, owner public key).
      //

      // compute the address.
      if (self.Create(this->family, this->component,
                      this->network,
                      this->family,
                      this->component,
                      this->_stamp, this->_salt, this->_owner.K) ==
          elle::Status::Error)
        escape("unable to compute the imprint address");

      // verify with the recorded address.
      if (address != self)
        escape("the address does not correspond to the block's public key");

      return elle::Status::Ok;
    }

    elle::utility::Time const&
    ImprintBlock::stamp() const
    {
      return (this->_stamp);
    }

    elle::cryptography::PublicKey const&
    ImprintBlock::owner_K() const
    {
      return (this->_owner.K);
    }

    neutron::Subject const&
    ImprintBlock::owner_subject()
    {
      // create a subject corresponding to the user, if necessary. note that
      // this subject will never be serialized hence is not really part of
      // the object but is used to ease the process of access control.
      if (this->_owner.subject == nullptr)
        {
          this->_owner.subject = new neutron::Subject;

          if (this->_owner.subject->Create(this->_owner.K) == elle::Status::Error)
            throw Exception("unable to create the owner subject");
        }

      assert(this->_owner.subject != nullptr);

      return (*this->_owner.subject);
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an block object.
    ///
    elle::Status
    ImprintBlock::Dump(const elle::Natural32 margin) const
    {
      elle::String alignment(margin, ' ');

      std::cout << alignment << "[ImprintBlock]" << std::endl;

      // dump the parent class.
      if (MutableBlock::Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the underlying block");

      // dump the stamp.
      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Stamp]" << std::endl;

      if (this->_stamp.Dump(margin + 4) == elle::Status::Error)
        escape("unable to dump the stamp");

      // dump the salt.
      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Salt] " << this->_salt << std::endl;

      // dump the owner.
      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Owner]" << std::endl;

      // dump the owner's public key.
      if (this->_owner.K.Dump(margin + 4) == elle::Status::Error)
        escape("unable to dump the owner's public key");

      // dump the subject.
      if (this->_owner.subject != nullptr)
        {
          if (this->_owner.subject->Dump(margin + 4) == elle::Status::Error)
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
