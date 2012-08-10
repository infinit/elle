#include <nucleus/neutron/Group.hh>
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
    }

    Group::Group(elle::cryptography::PublicKey const& owner_K,
                 elle::String const& description):
      ImprintBlock::ImprintBlock(ComponentGroup, owner_K),

      _description(description),
      _pass(nullptr)
    {
      this->state = proton::StateDirty;
    }

//
// ---------- methods ---------------------------------------------------------
//

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
              if (owner_k.Sign(elle::serialize::make_tuple(
                                 this->_description,
                                 *this->_pass,
                                 this->_ensemble),
                               this->_signature) == elle::Status::Error)
                throw Exception("unable to sign the group"); // XXX[to remove in the future]
            }
          else
            {
              if (owner_k.Sign(elle::serialize::make_tuple(
                                 this->_description,
                                 this->_ensemble),
                               this->_signature) == elle::Status::Error)
                throw Exception("unable to sign the group"); // XXX[to remove in the future]
            }

          // set the mutable block's version.
          this->version += 1;
        }

      this->state = proton::StateConsistent;
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

      if (this->_signature.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the signature");

      return (elle::Status::Ok);
    }

  }
}
