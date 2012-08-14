#include <nucleus/neutron/Group.hh>
#include <nucleus/neutron/Fellow.hh>
#include <nucleus/Exception.hh>

#include <elle/cryptography/KeyPair.hh>

namespace nucleus
{
  namespace neutron
  {

//
// ---------- construction ----------------------------------------------------
//

    Group::Group():
      ImprintBlock::ImprintBlock(ComponentGroup),

      _pass(nullptr)
    {
      this->_manager.fellow = nullptr;
    }

    Group::Group(elle::cryptography::PublicKey const& owner_K,
                 elle::String const& description):
      ImprintBlock::ImprintBlock(ComponentGroup, owner_K),

      _description(description),
      _pass(nullptr)
    {
      this->_manager.fellow = nullptr;

      this->state = proton::StateDirty;
    }

//
// ---------- methods ---------------------------------------------------------
//

    void
    Group::upgrade(proton::Address const& ensemble,
                   elle::cryptography::PublicKey const& pass)
    {
      delete this->_pass;
      this->_pass = new elle::cryptography::PublicKey(pass);

      this->_ensemble = ensemble;

      this->state = proton::StateDirty;
    }

    void
    Group::downgrade()
    {
      delete this->_pass;
      this->_pass = nullptr;

      this->_ensemble = proton::Address::Null;

      this->state = proton::StateDirty;
    }

    void
    Group::seal(elle::cryptography::PrivateKey const& owner_k)
    {
      if ((this->_ensemble != proton::Address::Null) &&
          (this->_pass == nullptr))
        throw Exception("unable to seal a group referencing an ensemble but "
                        "which does not embed a pass");

      if (this->state == proton::StateDirty)
        {
          // XXX[can this be improved with a single make_tuple()
          //     and elle::serialize::pointer()?]
          if (this->_pass != nullptr)
            {
              // XXX manager's token

              if (owner_k.Sign(elle::serialize::make_tuple(
                                 this->_description,
                                 *this->_pass,
                                 this->_ensemble),
                               this->_signature) == elle::Status::Error)
                throw Exception("unable to sign the group"); // XXX[to remove in the future]
            }
          else
            {
              // XXX delete manager's token

              if (owner_k.Sign(elle::serialize::make_tuple(
                                 this->_description,
                                 this->_ensemble),
                               this->_signature) == elle::Status::Error)
                throw Exception("unable to sign the group"); // XXX[to remove in the future]
            }

          // Increase the mutable block's version.
          this->version += 1;
        }

      // Now that the group has been sealed, the block can be considered
      // as consistent.
      this->state = proton::StateConsistent;
    }

    elle::cryptography::PublicKey const&
    Group::pass() const
    {
      assert(this->_pass != nullptr);

      return (*this->_pass);
    }

    proton::Address const&
    Group::ensemble() const
    {
      return (this->_ensemble);
    }

    Subject const&
    Group::manager_subject()
    {
      // The manager is also the owner of the block. In other words, both terms
      // are synonyms. Therefore, this method returns the underylying physical
      // block's owner subject.
      return (this->owner_subject());
    }

    Token const&
    Group::manager_token() const
    {
      return (this->_manager.token);
    }

    Fellow&
    Group::manager_fellow()
    {
      this->_manager_fellow();

      return (*this->_manager.fellow);
    }

    void
    Group::_manager_fellow()
    {
      // Create the fellow corresponding to the group manager, if necessary.
      // Note that this fellow will never be serialized but is used to ease
      // the process of group management since most method manipulate fellows.
      if (this->_manager.fellow == nullptr)
        this->_manager.fellow = new Fellow(this->manager_subject(),
                                           this->_manager.token);

      assert (this->_manager.fellow != nullptr);
    }

//
// ---------- dumpable --------------------------------------------------------
//

    elle::Status
    Group::Dump(const elle::Natural32 margin) const
    {
      elle::String alignment(margin, ' ');

      std::cout << alignment << "[Group]" << std::endl;

      if (ImprintBlock::Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the underlying block");

      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Description] " << this->_description << std::endl;

      if (this->_pass != nullptr)
        {
          if (this->_pass->Dump(margin + 2) == elle::Status::Error)
            escape("unable to dump the public pass");
        }
      else
        {
          std::cout << alignment << elle::io::Dumpable::Shift
                    << elle::io::Dumpable::Shift << "[Pass] "
                    << elle::none << std::endl;
        }

      if (this->_ensemble.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the ensemble's address");

      if (this->_manager.token.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the token");

      if (this->_manager.fellow != nullptr)
        {
          if (this->_manager.fellow->Dump(margin + 2) == elle::Status::Error)
            escape("unable to dump the fellow");
        }
      else
        {
          std::cout << alignment << elle::io::Dumpable::Shift
                    << elle::io::Dumpable::Shift << "[Fellow] "
                    << elle::none << std::endl;
        }

      if (this->_signature.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the signature");

      return (elle::Status::Ok);
    }

  }
}
