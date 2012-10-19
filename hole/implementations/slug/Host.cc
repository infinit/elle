#include <memory>

#include <elle/serialize/VectorSerializer.hxx>
#include <elle/utility/Time.hh>

#include <reactor/network/exception.hh>

#include <elle/Passport.hh>

#include <nucleus/Derivable.hh>
#include <nucleus/neutron/Access.hh>
#include <nucleus/neutron/Object.hh>
#include <nucleus/proton/ImmutableBlock.hh>
#include <nucleus/proton/MutableBlock.hh>

#include <hole/Hole.hh>
#include <hole/implementations/slug/Host.hh>
#include <hole/implementations/slug/Implementation.hh>
#include <hole/implementations/slug/Machine.hh>
#include <hole/implementations/slug/Manifest.hh>

#include <Infinit.hh>

ELLE_LOG_COMPONENT("infinit.hole.slug.Host");

namespace hole
{
  namespace implementations
  {
    namespace slug
    {
      /*-------------.
      | Construction |
      `-------------*/

      Host::Host(Machine& machine,
                 elle::network::Locus const& locus,
                 std::unique_ptr<reactor::network::Socket> socket,
                 bool opener)
        : _locus(locus)
        , _machine(machine)
        , _state(State::connected)
        , _socket(std::move(socket))
        , _serializer(elle::concurrency::scheduler(), *_socket)
        , _channels(elle::concurrency::scheduler(), _serializer, opener)
        , _rpcs(_channels)
        , _rpcs_handler(new reactor::Thread(elle::concurrency::scheduler(),
                                            elle::sprintf("RPC %s", *this),
                                            boost::bind(&Host::_rpc_run, this),
                                            true))
      {
        _rpcs.authenticate = boost::bind(&Host::_authenticate, this, _1);
        _rpcs.push = boost::bind(&Host::_push, this, _1, _2);
        _rpcs.pull = boost::bind(&Host::_pull, this, _1, _2);
        _rpcs.wipe = boost::bind(&Host::_wipe, this, _1);
      }

      Host::~Host()
      {}

      /*-----------.
      | Attributes |
      `-----------*/

      elle::network::Locus
      Host::locus() const
      {
        return this->_locus;
      }

      /*-----.
      | RPCs |
      `-----*/

      void
      Host::_rpc_run()
      {
        try
          {
            this->_rpcs.run();
          }
        catch (reactor::network::Exception& e)
          {
            ELLE_WARN("%s: discarded: %s", *this, e.what());
            this->_machine._remove(this);
          }
      }

      /*----.
      | API |
      `----*/

      void
      Host::push(nucleus::proton::Address const& address,
                 nucleus::proton::Block const& block)
      {
        ELLE_TRACE_SCOPE("%s: push block at address %s", *this, address);
        nucleus::Derivable derivable(address.component(), block);
        _rpcs.push(address, derivable);
      }

      std::unique_ptr<nucleus::proton::Block>
      Host::pull(nucleus::proton::Address const& address,
                 nucleus::proton::Revision const& revision)
      {
        ELLE_TRACE_SCOPE("%s: pull block at address %s with revision %s", *this, address, revision);
        return _rpcs.pull(address, revision).release();
      }

      void
      Host::wipe(nucleus::proton::Address const& address)
      {
        ELLE_TRACE_SCOPE("%s: wipe address %s", *this, address);
        _rpcs.wipe(address);
      }

      std::vector<elle::network::Locus>
      Host::authenticate(elle::Passport const& passport)
      {
        ELLE_TRACE_SCOPE("%s: authenticate with %s", *this, passport);
        _state = State::authenticating;
        return _rpcs.authenticate(passport);
      }

      std::vector<elle::network::Locus>
      Host::_authenticate(elle::Passport const& passport)
      {
        ELLE_TRACE_SCOPE("%s: peer authenticates with %s", *this, passport);

        assert(this->_state == State::connected
               || this->_state == State::authenticating);

        // Validate the passport.
        if (passport.Validate(this->_machine.hole().authority())
            == elle::Status::Error)
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "unable to validate the passport");
        else
          _state = State::authenticated;

        // Also authenticate to this host if we're not already doing so.
        if (this->_state == State::connected)
          authenticate(this->_machine.hole().passport());

        // Send back all the hosts we know.
        return _machine.loci();
      }

      void
      Host::_push(nucleus::proton::Address const& address,
                  nucleus::Derivable& derivable)
      {
        ELLE_TRACE_SCOPE("%s: peer pushes block at address %s", *this, address);

        if (this->_state != State::authenticated)
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "unable to process a request from an unauthenticated host");

        std::unique_ptr<nucleus::proton::Block> block = derivable.release();
        // Forward the request depending on the nature of the block
        // which the address indicates.
        switch (address.family())
          {
          case nucleus::proton::Family::content_hash_block:
            {
              ELLE_DEBUG("%s: block is immutable", *this);

              assert(dynamic_cast<nucleus::proton::ImmutableBlock*>(
                       block.get()));
              nucleus::proton::ImmutableBlock const& ib =
                static_cast<nucleus::proton::ImmutableBlock const&>(*block);

              this->_machine.hole().storage().store(address, ib);

              break;
            }
          case nucleus::proton::Family::public_key_block:
          case nucleus::proton::Family::owner_key_block:
          case nucleus::proton::Family::imprint_block:
            {
              assert(dynamic_cast<nucleus::proton::MutableBlock*>(block.get()));
              nucleus::proton::MutableBlock const* mb =
                static_cast<nucleus::proton::MutableBlock const*>(block.get());

              ELLE_DEBUG("%s: block is mutable", *this);
              {
                // Validate the block, depending on its component.
                // Indeed, the Object component requires as additional
                // block for being validated.
                switch (address.component())
                  {
                    case nucleus::neutron::ComponentObject:
                    {
                      assert(dynamic_cast<nucleus::neutron::Object const*>(mb));
                      nucleus::neutron::Object const* object =
                        static_cast<nucleus::neutron::Object const*>(mb);

                      ELLE_DEBUG("%s: validate the object block", *this);

                      // Validate the object according to the presence
                      // of a referenced access block.
                      if (object->access() != nucleus::proton::Address::null)
                        {
                          // Load the access block.
                          std::unique_ptr<nucleus::proton::Block> block
                            (this->_machine.hole().pull(object->access(),
                                                   nucleus::proton::Revision::Last));
                          std::unique_ptr<nucleus::neutron::Access> access
                            (dynamic_cast<nucleus::neutron::Access*>(block.release()));
                          if (access == nullptr)
                            throw reactor::Exception(elle::concurrency::scheduler(),
                                                     "expected an access block");

                          ELLE_DEBUG("%s: retrieve the access block", *this);

                          // Validate the object, providing the
                          object->validate(address, *access);
                        }
                      else
                        {
                          // Validate the object.
                          object->validate(address, nucleus::neutron::Access::Null);
                        }

                      break;
                    }
                  default:
                    {
                      ELLE_TRACE("%s: validate the mutable block", *this);

                      // Validate the block through the common interface.
                      mb->validate(address);

                      break;
                    }
                  case nucleus::neutron::ComponentUnknown:
                    {
                      throw reactor::Exception(elle::concurrency::scheduler(),
                                               elle::sprintf("unknown component '%u'",
                                                             address.component()));
                    }
                  }

                this->_machine.hole().storage().store(address, *mb);
              }
              break;
            }
          default:
            {
              throw reactor::Exception(elle::concurrency::scheduler(),
                                       "unknown block family");
            }
          }
      }

      nucleus::Derivable
      Host::_pull(nucleus::proton::Address const& address,
                  nucleus::proton::Revision const& revision)
      {
        ELLE_TRACE_SCOPE("%s: peer retreives block at address %s for revision %s",
                         *this, address, revision);

        using nucleus::proton::Block;
        using nucleus::proton::ImmutableBlock;
        using nucleus::proton::MutableBlock;

        if (this->_state != State::authenticated)
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "unable to process a request from an unauthenticated host");

        std::unique_ptr<Block> block;

        // forward the request depending on the nature of the block which
        // the addres indicates.
        switch (address.family())
          {
            case nucleus::proton::Family::content_hash_block:
            {
              ELLE_DEBUG("%s: block is immutable", *this);

              block = this->_machine.hole().storage().load(address);

              // validate the block.
              block->validate(address);

              break;
            }
            case nucleus::proton::Family::public_key_block:
            case nucleus::proton::Family::owner_key_block:
            case nucleus::proton::Family::imprint_block:
            {
              ELLE_TRACE("%s: block is mutable", *this);

              // Load block.
              block = this->_machine.hole().storage().load(address, revision);

              // validate the block, depending on its component.
              //
              // indeed, the Object component requires as additional
              // block for being validated.
              switch (address.component())
              {
                case nucleus::neutron::ComponentObject:
                {
                  const nucleus::neutron::Object* object =
                    static_cast<const nucleus::neutron::Object*>(block.get());
                  assert(dynamic_cast<const nucleus::neutron::Object*>(
                           block.get()) != nullptr);

                  // validate the object according to the presence of
                  // a referenced access block.
                  if (object->access() !=
                      nucleus::proton::Address::null)
                  {
                    // Load the access block.
                    std::unique_ptr<Block> addressBlock
                      (this->_machine.hole().storage().load(object->access(),
                                                            nucleus::proton::Revision::Last));

                    // Get address block.
                    nucleus::neutron::Access *access =
                      dynamic_cast<nucleus::neutron::Access *> (addressBlock.get());


                    if (access == nullptr)
                      throw reactor::Exception(elle::concurrency::scheduler(),
                                               "expected an access block");

                    // validate the object, providing the
                    object->validate(address, *access);
                  }
                  else
                  {
                    // validate the object.
                    object->validate(address, nucleus::neutron::Access::Null);
                  }

                  break;
                }
                default:
                {
                  // validate the block through the common interface.
                  block->validate(address);

                  break;
                }
                case nucleus::neutron::ComponentUnknown:
                {
                  throw reactor::Exception(elle::concurrency::scheduler(),
                                           elle::sprintf("unknown component '%u'",
                                                         address.component()));
                }
              }
              break;
            }
            default:
            {
              throw reactor::Exception(elle::concurrency::scheduler(),
                                       "unknown block family");
            }
          }

        return nucleus::Derivable(address.component(), *block.release());
      }

      void
      Host::_wipe(nucleus::proton::Address const& address)
      {
        ELLE_TRACE_SCOPE("%s: peer wipes block at address %s", *this, address);

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
          switch (address.family())
            {
            case nucleus::proton::Family::content_hash_block:
              {
                this->_machine.hole().storage().erase(address);

                break;
              }
            case nucleus::proton::Family::public_key_block:
            case nucleus::proton::Family::owner_key_block:
            case nucleus::proton::Family::imprint_block:
              {
                this->_machine.hole().storage().erase(address);

                break;
              }
            default:
              {
                throw reactor::Exception(elle::concurrency::scheduler(),
                                         "unknown block family");
              }
            }
        }
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

      void
      Host::print(std::ostream& stream) const
      {
        stream << "Host " << _locus;
      }

      std::ostream&
      operator << (std::ostream& stream, const Host& host)
      {
        host.print(stream);
        return stream;
      }
    }
  }
}
