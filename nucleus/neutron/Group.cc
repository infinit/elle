#include <nucleus/neutron/Group.hh>
#include <nucleus/neutron/Fellow.hh>
#include <nucleus/Exception.hh>

#include <elle/cryptography/KeyPair.hh>
#include <elle/cryptography/PrivateKey.hh>

#include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace neutron
  {

//
// ---------- construction ----------------------------------------------------
//

    Group::Group():
      ImprintBlock::ImprintBlock(ComponentGroup),

      _size(1)
    {
      this->_manager.fellow = nullptr;
    }

    Group::Group(elle::cryptography::PublicKey const& owner_K,
                 elle::String const& description):
      ImprintBlock::ImprintBlock(ComponentGroup, owner_K),

      _description(description),
      _size(1)
    {
      this->_manager.fellow = nullptr;

      this->state = proton::StateDirty;

      /* XXX[this is a hack which consists in generating a unique pass
             which will not evolve over time. */
      {
        elle::cryptography::KeyPair pass;

        if (pass.Generate() == elle::Status::Error)
          throw Exception("unable to generate the pass"); // XXX[to remove in the future]

        Token token(this->manager_subject().user(), pass.k);
        this->upgrade(proton::Address::Null, pass.K, token);
      }
    }

//
// ---------- methods ---------------------------------------------------------
//

    void
    Group::upgrade(proton::Address const& ensemble,
                   elle::cryptography::PublicKey const& pass_K,
                   Token const& manager_token)
    {
      this->_pass_K = pass_K;

      if (this->_modification_stamp.Current() == elle::Status::Error)
        throw Exception("unable to set the last management time"); // XXX[to remove later]

      this->_ensemble = ensemble;
      this->_manager.token = manager_token;

      this->state = proton::StateDirty;
    }

    void
    Group::downgrade()
    {
      if (this->_modification_stamp.Current() == elle::Status::Error)
        throw Exception("unable to set the last management time"); // XXX[to remove later]

      this->_ensemble = proton::Address::Null;

      this->state = proton::StateDirty;
    }

    void
    Group::seal(elle::cryptography::PrivateKey const& owner_k)
    {
      switch (this->state)
        {
        case proton::StateClean:
        // XXX[duplicate case for now] case proton::StateConsistent:
          {
            // Nothing to do, the group is already valid.

            break;
          }
        case proton::StateDirty:
          {
            // XXX[can this be improved with a single make_tuple()
            //     and elle::serialize::pointer()? though we do not have
            //     pointers left here!]
            if (owner_k.Sign(elle::serialize::make_tuple(
                               this->_description,
                               this->_pass_K,
                               this->_size,
                               this->_modification_stamp,
                               this->_ensemble,
                               this->_manager.token),
                             this->_signature) == elle::Status::Error)
              throw Exception("unable to sign the group"); // XXX[to remove in the future]

            // Increase the mutable block's version.
            this->version += 1;

            break;
          }
        }

      // Now that the group has been sealed, the block can be considered
      // as consistent.
      this->state = proton::StateConsistent;
    }

    elle::String const&
    Group::description() const
    {
      return (this->_description);
    }

    elle::cryptography::PublicKey const&
    Group::pass_K() const
    {
      return (this->_pass_K);
    }

    proton::Address const&
    Group::ensemble() const
    {
      return (this->_ensemble);
    }

    elle::cryptography::PublicKey const&
    Group::manager_K() const
    {
      return (this->owner_K());
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
      assert(this->_manager.token != Token::Null);

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

    Size const&
    Group::size() const
    {
      return (this->_size);
    }

    void
    Group::size(Size const& size)
    {
      this->_size = size;
    }

    elle::utility::Time const&
    Group::modification_stamp() const
    {
      return (this->_modification_stamp);
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

      if (this->_pass_K.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the public pass");

      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Size] " << std::dec << this->_size << std::endl;

      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Modifcation Time]" << std::endl;

      if (this->_modification_stamp.Dump(margin + 4) == elle::Status::Error)
        escape("unable to dump the stamp");

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
