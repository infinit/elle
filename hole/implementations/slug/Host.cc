#include <memory>

#include <elle/utility/Time.hh>
#include <elle/standalone/Morgue.hh>
#include <elle/network/TCPSocket.hh>

#include <nucleus/neutron/Access.hh>
#include <nucleus/neutron/Object.hh>
#include <nucleus/proton/ImmutableBlock.hh>
#include <nucleus/proton/MutableBlock.hh>

#include <hole/Hole.hh>
#include <hole/Holeable.hh>
#include <hole/implementations/slug/Host.hh>
#include <hole/implementations/slug/Machine.hh>
#include <hole/implementations/slug/Manifest.hh>
#include <hole/implementations/slug/Slug.hh>

#include <Infinit.hh>

ELLE_LOG_TRACE_COMPONENT("Infinit.Hole.Slug.Host");

namespace hole
{
  namespace implementations
  {
    namespace slug
    {
      /// XXX 2 sec
      const elle::Natural32             Host::Timeout = 1000;


      /*-------------.
      | Construction |
      `-------------*/

      Host::Host(Machine& machine, std::unique_ptr<reactor::network::Socket> socket, bool opener)
        : _machine(machine)
        , _state(State::connected)
        , _socket(std::move(socket))
        , _serializer(elle::concurrency::scheduler(), *_socket)
        , _channels(elle::concurrency::scheduler(), _serializer, opener)
        , _rpcs(_channels)
        , _rpcs_handler(elle::concurrency::scheduler(),
                        elle::sprintf("RPCs handler for %s", *this),
                        boost::bind(&RPC::run, &_rpcs))
      {
        _rpcs.authenticate = boost::bind(&Host::_authenticate, this, _1);
        _rpcs.push = boost::bind(&Host::_push, this, _1, _2);
        _rpcs.pull = boost::bind(&Host::_pull, this, _1, _2);
        _rpcs.wipe = boost::bind(&Host::_wipe, this, _1);
      }

      Host::~Host()
      {}

      /*----.
      | API |
      `----*/

      bool
      Host::push(nucleus::proton::Address const& address,
                 nucleus::proton::Block const& block)
      {
        nucleus::Derivable derivable(address.component, block);
        return _rpcs.push(address, derivable);
      }

      std::unique_ptr<nucleus::proton::MutableBlock>
      Host::pull(nucleus::proton::Address const& address,
                 nucleus::proton::Version const& version)
      {
        std::unique_ptr<nucleus::proton::Block> block(_rpcs.pull(address, version).release());
        assert(dynamic_cast<nucleus::proton::MutableBlock*>(block.get()));
        return std::unique_ptr<nucleus::proton::MutableBlock>(static_cast<nucleus::proton::MutableBlock*>(block.release()));
      }

      bool
      Host::wipe(nucleus::proton::Address const& address)
      {
        return _rpcs.wipe(address);
      }

      std::vector<elle::network::Locus>
      Host::authenticate(lune::Passport& passport)
      {
        _state = State::authenticating;
        return _rpcs.authenticate(passport);
      }

      std::vector<elle::network::Locus>
      Host::_authenticate(lune::Passport const& passport)
      {
        ELLE_LOG_TRACE("Authenticate");

        assert(this->_state == State::connected || this->_state == State::authenticating);

        // Validate the passport.
        if (passport.Validate(Infinit::Authority) == elle::Status::Error)
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "unable to validate the passport");
        else
          _state = State::authenticated;

        // Also authenticate to this host if we're not already doing so.
        if (this->_state == State::connected)
          authenticate(Hole::Passport);

        // Send back all the hosts we know.
        return _machine.loci();
      }

      bool
      Host::_push(nucleus::proton::Address const& address,
                  nucleus::Derivable& derivable)
      {
        ELLE_LOG_TRACE_SCOPE("Push");

        if (this->_state != State::authenticated)
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "unable to process a request from an unauthenticated host");

        std::unique_ptr<nucleus::proton::Block> block = derivable.release();
        // Forward the request depending on the nature of the block
        // which the address indicates.
        switch (address.family)
          {
            case nucleus::proton::FamilyContentHashBlock:
            {
              assert(dynamic_cast<nucleus::proton::ImmutableBlock*>(
                       block.get()));
              nucleus::proton::ImmutableBlock const& ib =
                static_cast<nucleus::proton::ImmutableBlock const&>(*block);

              ELLE_LOG_TRACE("Push immutable block");

              if (ib.Exist(Hole::Implementation->network,
                           address) == elle::Status::True)
                throw reactor::Exception(elle::concurrency::scheduler(),
                                         "this immutable block seems to already exist");
              if (ib.Store(Hole::Implementation->network,
                           address) == elle::Status::Error)
                throw reactor::Exception(elle::concurrency::scheduler(),
                                         "unable to store the block");
              break;
            }
          case nucleus::proton::FamilyPublicKeyBlock:
          case nucleus::proton::FamilyOwnerKeyBlock:
          case nucleus::proton::FamilyImprintBlock:
            {
              assert(dynamic_cast<nucleus::proton::MutableBlock*>(
                       block.get()));
              nucleus::proton::MutableBlock const* mb =
                static_cast<nucleus::proton::MutableBlock const*>(block.get());

              ELLE_LOG_TRACE("pushing mutable block")
              {
                // Validate the block, depending on its component.
                // Indeed, the Object component requires as additional
                // block for being validated.
                switch (address.component)
                  {
                    case nucleus::neutron::ComponentObject:
                    {
                      assert(dynamic_cast<nucleus::neutron::Object const*>(mb));
                      nucleus::neutron::Object const* object =
                        static_cast<nucleus::neutron::Object const*>(mb);

                      ELLE_LOG_TRACE("validating the object mutable block");

                      // Validate the object according to the presence
                      // of a referenced access block.
                      if (object->meta.access != nucleus::proton::Address::Null)
                        {
                          // Load the access block.
                          std::unique_ptr<nucleus::proton::Block> block
                            (Hole::Pull(object->meta.access, nucleus::proton::Version::Last));
                          std::unique_ptr<nucleus::neutron::Access> access
                            (dynamic_cast<nucleus::neutron::Access*>(block.release()));
                          if (access == nullptr)
                            throw reactor::Exception(elle::concurrency::scheduler(),
                                                     "expected an access block");

                          ELLE_LOG_TRACE("retrieving the access block");

                          // Validate the object, providing the
                          if (object->Validate(address,
                                               *access) == elle::Status::Error)
                            throw reactor::Exception(elle::concurrency::scheduler(),
                                                     "unable to validate the object");
                        }
                      else
                        {
                          // Validate the object.
                          if (object->Validate(
                                address,
                                nucleus::neutron::Access::Null) == elle::Status::Error)
                            throw reactor::Exception(elle::concurrency::scheduler(),
                                                     "unable to validate the object");
                        }

                      break;
                    }
                  default:
                    {
                      ELLE_LOG_TRACE("validating the mutable block");

                      // Validate the block through the common interface.
                      if (mb->Validate(address) == elle::Status::Error)
                        throw reactor::Exception(elle::concurrency::scheduler(),
                                                 "the block seems to be invalid");

                      break;
                    }
                  case nucleus::neutron::ComponentUnknown:
                    {
                      throw reactor::Exception(elle::concurrency::scheduler(),
                                               elle::sprintf("unknown component '%u'",
                                                             address.component));
                    }
                  }

                // Does the block already exist.
                if (mb->Exist(Hole::Implementation->network,
                              address,
                              nucleus::proton::Version::Last) == elle::Status::True)
                  {
                    nucleus::proton::MutableBlock* current;

                    ELLE_LOG_TRACE("the mutable block seems to exist "
                                   "locally, make sure it derives the "
                                   "current version");

                    // build a block according to the component.
                    if (nucleus::Nucleus::Factory.Build(
                          address.component,
                          current) == elle::Status::Error)
                      throw reactor::Exception(elle::concurrency::scheduler(), "unable to build the block");

                    std::unique_ptr<nucleus::proton::MutableBlock> guard(current);

                    // load the latest version.
                    if (current->Load(Hole::Implementation->network,
                                      address,
                                      nucleus::proton::Version::Last) ==
                        elle::Status::Error)
                      throw reactor::Exception(elle::concurrency::scheduler(), "unable to load the current version");

                    // does the given block derive the current version.
                    if (!(mb->version > current->version))
                      throw reactor::Exception(elle::concurrency::scheduler(), "the block to store does not seem to derive "
                             "the current version");
                  }

                ELLE_LOG_TRACE("now storing the validated mutable block locally");

                // store the block.
                if (mb->Store(Hole::Implementation->network,
                              address) == elle::Status::Error)
                  throw reactor::Exception(elle::concurrency::scheduler(), "unable to store the block");
              }

              break;
            }
          default:
            {
              throw reactor::Exception(elle::concurrency::scheduler(), "unknown block family");
            }
          }
        return true;
      }

      nucleus::Derivable
      Host::_pull(nucleus::proton::Address const& address,
                  nucleus::proton::Version const& version)
      {
        using nucleus::proton::Block;
        using nucleus::proton::ImmutableBlock;
        using nucleus::proton::MutableBlock;

        Block* raw_block;

        ELLE_LOG_TRACE("Pull");

        if (this->_state != State::authenticated)
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "unable to process a request from an unauthenticated host");

        // build the block according to the component.
        if (nucleus::Nucleus::Factory.Build(address.component,
                                            raw_block) == elle::Status::Error)
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "unable to build the block");

        std::unique_ptr<Block> block(raw_block);

        // forward the request depending on the nature of the block which
        // the addres indicates.
        switch (address.family)
          {
          case nucleus::proton::FamilyContentHashBlock:
            {
              ImmutableBlock* ib;

              ELLE_LOG_TRACE("pulling immutable block");

              // Cast to an immutable block.
              assert(dynamic_cast<ImmutableBlock*>(block.get()) != nullptr);
              ib = static_cast<ImmutableBlock*>(block.get());

              // does the block exist.
              if (ib->Exist(Hole::Implementation->network,
                            address) == elle::Status::False)
                throw reactor::Exception(elle::concurrency::scheduler(),
                                         "the block does not seem to exist");

              // load the block.
              if (ib->Load(Hole::Implementation->network,
                           address) == elle::Status::Error)
                throw reactor::Exception(elle::concurrency::scheduler(),
                                         "unable to load the block");

              // validate the block.
              if (ib->Validate(address) == elle::Status::Error)
                throw reactor::Exception(elle::concurrency::scheduler(),
                                         "the block seems to be invalid");

              break;
            }
          case nucleus::proton::FamilyPublicKeyBlock:
          case nucleus::proton::FamilyOwnerKeyBlock:
          case nucleus::proton::FamilyImprintBlock:
            {
              MutableBlock* mb;

              ELLE_LOG_TRACE("pulling mutable block");

              assert(dynamic_cast<MutableBlock*>(block.get()) != nullptr);
              mb = static_cast<MutableBlock*>(block.get());
              if (mb->Exist(Hole::Implementation->network,
                            address, version) == elle::Status::True)
                {
                  // load the block.
                  if (mb->Load(Hole::Implementation->network,
                               address, version) == elle::Status::Error)
                    throw reactor::Exception(elle::concurrency::scheduler(),
                                             "unable to load the block");

                  // validate the block, depending on its component.
                  //
                  // indeed, the Object component requires as additional
                  // block for being validated.
                  switch (address.component)
                    {
                    case nucleus::neutron::ComponentObject:
                      {
                        const nucleus::neutron::Object* object =
                          static_cast<const nucleus::neutron::Object*>(mb);
                        assert(dynamic_cast<const nucleus::neutron::Object*>(
                                 mb) != nullptr);

                        // validate the object according to the presence of
                        // a referenced access block.
                        if (object->meta.access !=
                            nucleus::proton::Address::Null)
                          {
                            // Load the access block.
                            std::unique_ptr<Block> block
                              (Hole::Pull(object->meta.access, nucleus::proton::Version::Last));
                            std::unique_ptr<nucleus::neutron::Access> access
                              (dynamic_cast<nucleus::neutron::Access*>(block.release()));
                            if (access == nullptr)
                              throw reactor::Exception(elle::concurrency::scheduler(),
                                                       "expected an access block");

                            // validate the object, providing the
                            if (object->Validate(address,
                                                 *access) == elle::Status::Error)
                              throw reactor::Exception(elle::concurrency::scheduler(),
                                                       "unable to validate the object");
                          }
                        else
                          {
                            // validate the object.
                            if (object->Validate(
                                  address,
                                  nucleus::neutron::Access::Null) == elle::Status::Error)
                              throw reactor::Exception(elle::concurrency::scheduler(),
                                                       "unable to validate the object");
                          }

                        break;
                      }
                    default:
                      {
                        // validate the block through the common interface.
                        if (mb->Validate(address) == elle::Status::Error)
                          throw reactor::Exception(elle::concurrency::scheduler(),
                                                   "the block seems to be invalid");

                        break;
                      }
                    case nucleus::neutron::ComponentUnknown:
                      {
                        throw reactor::Exception(elle::concurrency::scheduler(),
                                                 elle::sprintf("unknown component '%u'",
                                                               address.component));
                      }
                    }
                }
              else
                throw reactor::Exception(
                  elle::concurrency::scheduler(),
                  elle::sprintf("pulled block does not exist: %s", address));
              break;
            }
          default:
            {
              throw reactor::Exception(elle::concurrency::scheduler(),
                                       "unknown block family");
            }
          }
        return nucleus::Derivable(address.component, *block.release());
      }

      bool
      Host::_wipe(nucleus::proton::Address const& address)
      {
        ELLE_LOG_TRACE_SCOPE("Wipe");

        // check the host's state.
        if (this->_state != State::authenticated)
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "unable to process a request from an unauthenticated host");

        //
        // remove the block locally.
        //
        {
          // treat the request depending on the nature of the block which
          // the addres indicates.
          switch (address.family)
            {
            case nucleus::proton::FamilyContentHashBlock:
              {
                // erase the immutable block.
                if (nucleus::proton::ImmutableBlock::Erase(
                      Hole::Implementation->network,
                      address) == elle::Status::Error)
                  throw reactor::Exception(elle::concurrency::scheduler(),
                                           "unable to erase the block");
                break;
              }
            case nucleus::proton::FamilyPublicKeyBlock:
            case nucleus::proton::FamilyOwnerKeyBlock:
            case nucleus::proton::FamilyImprintBlock:
              {
                // retrieve the mutable block.
                if (nucleus::proton::MutableBlock::Erase(
                      Hole::Implementation->network,
                      address) == elle::Status::Error)
                  throw reactor::Exception(elle::concurrency::scheduler(),
                                           "unable to erase the block");
                break;
              }
            default:
              {
                throw reactor::Exception(elle::concurrency::scheduler(),
                                         "unknown block family");
              }
            }
        }
        return true;
      }

      /*---------.
      | Dumpable |
      `---------*/

      static std::ostream&
      operator << (std::ostream& stream, Host::State state)
      {
        switch (state)
          {
            case Host::State::connected:
              stream << "connected";
              break;
            case Host::State::authenticating:
              stream << "authenticating";
              break;
            case Host::State::authenticated:
              stream << "authenticated";
              break;
            case Host::State::dead:
              stream << "dead";
              break;
          }
        return stream;
      }

      elle::Status      Host::Dump(elle::Natural32      margin) const
      {
        elle::String    alignment(margin, ' ');
        std::cout << alignment << "[Host] " << std::hex << this << std::endl
                  << alignment << elle::io::Dumpable::Shift
                  << "[State] " << std::dec << this->_state << std::endl;
        return elle::Status::Ok;
      }

    }
  }
}
