#include <nucleus/proton/ImprintBlock.hh>
#include <nucleus/proton/Address.hh>

#include <elle/cryptography/Random.hh>
#include <elle/cryptography/PublicKey.hh>

#include <nucleus/proton/Family.hh>

#include <elle/idiom/Open.hh>

ELLE_LOG_COMPONENT("Nucleus.Proton");

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
    ImprintBlock::ImprintBlock():
      MutableBlock()
    {
    }

    ///
    /// specific constructor.
    ///
    ImprintBlock::ImprintBlock(const neutron::Component         component):
      MutableBlock(FamilyImprintBlock, component)
    {
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
    elle::Status        ImprintBlock::Create(elle::cryptography::PublicKey const& owner)
    {
      ELLE_TRACE("Creating an ImprintBlock %p", this)
      {
        // retrieve the current time.
        if (this->stamp.Current() == elle::Status::Error)
          escape("unable to retrieve the current time");

        // generate a random number.
        if (elle::cryptography::Random::Generate(this->salt) == elle::Status::Error)
          escape("unable to generate the seed");

        // set the owner public key.
        assert(owner.key() != nullptr);
        this->owner.K = owner;

        // create a subject corresponding to the user. note that this
        // subject will never be serialized hence is not really part of
        // the object but is used to ease the process of access control.
        if (this->owner.subject.Create(this->owner.K) == elle::Status::Error)
          escape("unable to create the owner subject");

      }
      return elle::Status::Ok;
    }

    ///
    /// this method computes the block's address.
    ///
    elle::Status        ImprintBlock::Bind(Address&             address)
      const
    {
      ELLE_TRACE("Binding an ImprintBlock %p", this)
      {
        assert(this->owner.K.key() != nullptr);
        // compute the address.
        if (address.Create(this->family, this->component,
                           this->network,
                           this->family,
                           this->component,
                           this->stamp, this->salt, this->owner.K) ==
            elle::Status::Error)
          escape("unable to compute the imprint address");
      }

      return elle::Status::Ok;
    }

    ///
    /// this method verifies the block's validity.
    ///
    elle::Status        ImprintBlock::Validate(const Address&   address)
      const
    {
      Address           self;
      ELLE_TRACE("Validating an ImprintBlock address %p", this)
      {
        assert(this->owner.K.key() != nullptr);
        //
        // make sure the address has not be tampered and correspond to the
        // hash of the tuple (stamp, salt, owner public key).
        //

        // compute the address.
        if (self.Create(this->family, this->component,
                        this->network,
                        this->family,
                        this->component,
                        this->stamp, this->salt, this->owner.K) ==
            elle::Status::Error)
          escape("unable to compute the imprint address");

        // verify with the recorded address.
        if (address != self)
          escape("the address does not correspond to the block's public key");
      }

      return elle::Status::Ok;
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an block object.
    ///
    elle::Status        ImprintBlock::Dump(const
                                             elle::Natural32    margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[ImprintBlock]" << std::endl;

      // dump the parent class.
      if (MutableBlock::Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the underlying block");

      // dump the stamp.
      std::cout << alignment << elle::io::Dumpable::Shift
                << elle::io::Dumpable::Shift
                << "[Stamp]" << std::endl;

      if (this->stamp.Dump(margin + 6) == elle::Status::Error)
        escape("unable to dump the stamp");

      // dump the salt.
      std::cout << alignment << elle::io::Dumpable::Shift
                << elle::io::Dumpable::Shift
                << "[Salt] " << this->salt << std::endl;

      // dump the owner.
      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Owner]" << std::endl;

      // dump the owner's public key.
      if (this->owner.K.Dump(margin + 4) == elle::Status::Error)
        escape("unable to dump the owner's public key");

      // dump the subject.
      if (this->owner.subject.Dump(margin + 4) == elle::Status::Error)
        escape("unable to dump the subject");

      return elle::Status::Ok;
    }

  }
}
