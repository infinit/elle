#include <nucleus/neutron/Group.hh>
#include <nucleus/neutron/Fellow.hh>
#include <nucleus/Exception.hh>

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

      _pass(nullptr),
      _size(1)
    {
      this->_manager.token = nullptr;
      this->_manager.fellow = nullptr;
    }

    Group::Group(elle::cryptography::PublicKey const& owner_K,
                 elle::String const& description):
      ImprintBlock::ImprintBlock(ComponentGroup, owner_K),

      _description(description),
      _pass(nullptr),
      _size(1)
    {
      this->_manager.token = nullptr;
      this->_manager.fellow = nullptr;

      this->state = proton::StateDirty;
    }

//
// ---------- methods ---------------------------------------------------------
//

    void
    Group::upgrade(proton::Address const& ensemble,
                   elle::cryptography::PublicKey const& pass_K,
                   elle::cryptography::PrivateKey const& pass_k)
    {
      delete this->_pass;
      this->_pass = new elle::cryptography::PublicKey(pass_K);

      if (this->_modification_stamp.Current() == elle::Status::Error)
        throw Exception("unable to set the last management time"); // XXX[to remove later]

      this->_ensemble = ensemble;

      delete this->_manager.token;
      this->_manager.token = new Token(this->manager_subject().user(), pass_k);

      this->state = proton::StateDirty;
    }

    void
    Group::downgrade()
    {
      delete this->_pass;
      this->_pass = nullptr;

      if (this->_modification_stamp.Current() == elle::Status::Error)
        throw Exception("unable to set the last management time"); // XXX[to remove later]

      this->_ensemble = proton::Address::Null;

      // XXX[note that, for the moment, the token is not reset to null, though
      //     it should. this is required because, for now, the manager needs to
      //     be able to retrieve the private pass at any time, from its token
      //     because the pass is never re-generated. this is a temporary hack.]

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
              if (this->_manager.token != nullptr)
                {
                  if (owner_k.Sign(elle::serialize::make_tuple(
                                     this->_description,
                                     *this->_pass,
                                     this->_size,
                                     this->_modification_stamp,
                                     this->_ensemble,
                                     *this->_manager.token),
                                   this->_signature) == elle::Status::Error)
                    throw Exception("unable to sign the group"); // XXX[to remove in the future]
                }
              else
                {
                  if (owner_k.Sign(elle::serialize::make_tuple(
                                     this->_description,
                                     *this->_pass,
                                     this->_size,
                                     this->_modification_stamp,
                                     this->_ensemble),
                                   this->_signature) == elle::Status::Error)
                    throw Exception("unable to sign the group"); // XXX[to remove in the future]
                }
            }
          else
            {
              if (this->_manager.token != nullptr)
                {
                  if (owner_k.Sign(elle::serialize::make_tuple(
                                     this->_description,
                                     this->_size,
                                     this->_modification_stamp,
                                     this->_ensemble,
                                     *this->_manager.token),
                                   this->_signature) == elle::Status::Error)
                    throw Exception("unable to sign the group"); // XXX[to remove in the future]
                }
              else
                {
                  if (owner_k.Sign(elle::serialize::make_tuple(
                                     this->_description,
                                     this->_size,
                                     this->_modification_stamp,
                                     this->_ensemble),
                                   this->_signature) == elle::Status::Error)
                    throw Exception("unable to sign the group"); // XXX[to remove in the future]
                }
            }

          // Increase the mutable block's version.
          this->version += 1;
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
      assert(this->_manager.token != nullptr);

      return (*this->_manager.token);
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
        {
          if (this->_manager.token != nullptr)
            this->_manager.fellow = new Fellow(this->manager_subject(),
                                               *this->_manager.token);
          else
            this->_manager.fellow = new Fellow(this->manager_subject());
        }

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

      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Size] " << std::dec << this->_size << std::endl;

      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Modifcation Time]" << std::endl;

      if (this->_modification_stamp.Dump(margin + 4) == elle::Status::Error)
        escape("unable to dump the stamp");

      if (this->_ensemble.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the ensemble's address");

      if (this->_manager.token != nullptr)
        {
          if (this->_manager.token->Dump(margin + 2) == elle::Status::Error)
            escape("unable to dump the token");
        }
      else
        {
          std::cout << alignment << elle::io::Dumpable::Shift
                    << elle::io::Dumpable::Shift << "[Token] "
                    << elle::none << std::endl;
        }

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
