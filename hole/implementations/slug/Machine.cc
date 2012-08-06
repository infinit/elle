#include <hole/Hole.hh>
#include <hole/Holeable.hh>
#include <hole/implementations/slug/Host.hh>
#include <hole/implementations/slug/Machine.hh>
#include <hole/implementations/slug/Manifest.hh>

#include <reactor/network/exception.hh>
#include <reactor/network/tcp-server.hh>

#include <agent/Agent.hh>

#include <lune/Passport.hh>

#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Block.hh>
#include <nucleus/proton/Version.hh>
#include <nucleus/proton/ImmutableBlock.hh>
#include <nucleus/proton/MutableBlock.hh>
#include <nucleus/neutron/Object.hh>
#include <nucleus/neutron/Access.hh>
#include <nucleus/Derivable.hh>

#include <elle/network/Network.hh>
#include <elle/network/Inputs.hh>
#include <elle/network/Outputs.hh>
#include <elle/standalone/Report.hh>
#include <elle/standalone/Morgue.hh>
#include <elle/network/TCPSocket.hh>

#include <Infinit.hh>

#include <plasma/meta/Client.hh>

#include <common/common.hh>

#include <elle/log.hh>

ELLE_LOG_TRACE_COMPONENT("Infinit.Hole.Slug.Machine");

namespace hole
{
  namespace implementations
  {
    namespace slug
    {
      /// XXX 3 seconds
      const reactor::Duration Machine::Timeout = boost::posix_time::seconds(3);

      /*-------------.
      | Construction |
      `-------------*/

      void
      Machine::_connect(elle::network::Locus const& locus)
      {
        std::string hostname;
        locus.host.Convert(hostname);
        std::unique_ptr<reactor::network::Socket> socket(
          new reactor::network::TCPSocket(elle::concurrency::scheduler(),
                                          hostname, locus.port));
        _connect(std::move(socket), locus, true);
      }

      void
      Machine::_connect(std::unique_ptr<reactor::network::Socket> socket,
                        elle::network::Locus const& locus, bool opener)
      {
        std::unique_ptr<Host> host(new Host(*this, std::move(socket), opener));
        auto loci = host->authenticate(Hole::Passport);
        this->_state = State::attached;
        if (Hole::Ready() == elle::Status::Error)
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "unable to set the hole online");
        for (auto locus: loci)
          if (_hosts.find(locus) == _hosts.end())
            _connect(locus);
        _hosts[locus] = host.release();
      }

      Machine::Machine()
        : _state(State::detached)
        , _port(0)
        , _server(new reactor::network::TCPServer
                  (elle::concurrency::scheduler()))
      {
        elle::network::Locus     locus;
        ELLE_LOG_TRACE_SCOPE("launch");

        // Connect to hosts from the descriptor.
        {
          std::istringstream    stream;
          this->_port = Infinit::Configuration["hole"].Get("slug.port", 0);

          // FIXME: try these in parallel
          for (elle::network::Locus& locus: Hole::Set.loci)
            try
              {
                ELLE_LOG_TRACE("try connecting to %s", locus)
                  _connect(locus);
              }
            catch (reactor::network::Exception& err)
              {
                ELLE_LOG_TRACE("ignore host %s", locus);
                continue;
              }
        }

        // Set up the timeout after which the machine will be
        // considered alone in the network, in other words, the very
        // first node.
        elle::concurrency::scheduler().CallLater
          (boost::bind(&Machine::_alone, this),
           "Machine::Alone", Machine::Timeout);

        // Finally, listen for incoming connections.
        {
          elle::network::Locus locus;
          elle::network::Host host(elle::network::Host::TypeAny);
          try
            {
              _server->listen(this->_port);
              // In case we asked for a random port to be picked up
              // (by using 0), retrieve the actual listening port.
              this->_port = _server->local_endpoint().port();
              {
                // XXX should be done with a signal
                plasma::meta::Client client("127.0.0.1", 23456);
                lune::Passport passport;
                if (passport.Load() != elle::Status::Ok)
                  throw reactor::Exception(elle::concurrency::scheduler(),
                                           "Cannot load passport");
                try
                  {
                    client.token(agent::Agent::meta_token);
                    client.update_device(passport.id, nullptr, nullptr, this->_port);
                  }
                catch (std::exception const& err)
                  {
                    ELLE_LOG_TRACE("Cannot update device port: %s",
                                   err.what()); // XXX[to improve]
                  }
              }
              new reactor::Thread(elle::concurrency::scheduler(),
                                  "Slug accept",
                                  boost::bind(&Machine::_accept, this),
                                  true);
            }
          catch (reactor::Exception& e)
            {
              ELLE_LOG_TRACE("unable to listen: %s", e.what());
              // FIXME: what do ? For now, just go on without
              // listening. Useful when testing with several clients
              // on the same machine.
            }
        }
      }

      Machine::~Machine()
      {}

      /*------.
      | Hosts |
      `------*/

      std::vector<elle::network::Locus>
      Machine::loci()
      {
        std::vector<elle::network::Locus> res;
        for (auto host: _hosts)
          res.push_back(host.first);
        return std::move(res);
      }

      std::vector<Host*>
      Machine::hosts()
      {
        std::vector<Host*> res;
        for (auto host: _hosts)
          res.push_back(host.second);
        return std::move(res);
      }

      /*-------.
      | Server |
      `-------*/

      elle::network::Port
      Machine::port() const
      {
        return this->_port;
      }

      void
      Machine::_accept()
      {
        while (true)
          {
            std::unique_ptr<reactor::network::TCPSocket> socket(_server->accept());

            ELLE_LOG_TRACE_SCOPE("accept connection from %s", socket->peer());

            // Depending on the machine's state.
            switch (this->_state)
              {
                case State::attached:
                {
                  // FIXME: handling via loci is very wrong. IPs are
                  // not uniques, and this reconstruction is lame and
                  // non-injective.
                  elle::network::Locus locus(
                    socket->peer().address().to_string(),
                    socket->peer().port());
                  _connect(std::move(socket), locus, false);
                  break;
                }
                default:
                {
                  // FIXME: Why not listening only when we're attached ?
                  ELLE_LOG_TRACE("not attached, ignore %s", socket->peer());
                  break;
                }
              }
          }
      }

      /*----.
      | API |
      `----*/

      void
      Machine::Put(const nucleus::proton::Address&    address,
                   const nucleus::proton::ImmutableBlock& block)
      {
        ELLE_LOG_TRACE("Put[Immutable]");

        // depending on the machine's state.
        switch (this->_state)
          {
            case State::attached:
            {
              // In this case, the current machine is connected and
              // has been authenticated as a valid node of the
              // network.  Therefore, the operation is carried out
              // both locally but also sent to every node in the
              // network.

              // Store the block locally.
              {
                if (block.Exist(Hole::Implementation->network,
                                address) == elle::Status::True)
                  throw reactor::Exception(elle::concurrency::scheduler(),
                                           "this immutable block seems to already exist");

                if (block.Validate(address) == elle::Status::Error)
                  throw reactor::Exception(elle::concurrency::scheduler(),
                                           "unable to validate the block");

                if (block.Store(Hole::Implementation->network,
                                address) == elle::Status::Error)
                  throw reactor::Exception(elle::concurrency::scheduler(),
                                           "unable to store the block");
              }
              // Publish it onto the network.
              {
                for (auto neighbour: _hosts)
                  {
                    Host* host = neighbour.second;
                    host->push(address, block);
                    // Ignore the error messages and continue with the
                    // next neighbour.
                    purge();
                  }
              }

              break;
            }
          default:
            {
              throw reactor::Exception
                (elle::concurrency::scheduler(),
                 elle::sprintf("the machine's state '%u' does not allow one to request "
                               "operations on the storage layer",
                               this->_state));
            }
          }
      }

      void
      Machine::Put(const nucleus::proton::Address&    address,
                   const nucleus::proton::MutableBlock& block)
      {
        ELLE_LOG_TRACE("Put[Mutable]");

        // depending on the machine's state.
        switch (this->_state)
          {
            case State::attached:
            {
              //
              // in this case, the current machine is connected and has
              // been authenticated as a valid node of the network.
              //
              // therefore, the operation is carried out both locally but
              // also sent to every node in the network.
              //

              //
              // first, store the block locally.
              //
              {
                // validate the block, depending on its component.
                //
                // indeed, the Object component requires as additional
                // block for being validated.
                switch (address.component)
                  {
                  case nucleus::neutron::ComponentObject:
                    {
                      const nucleus::neutron::Object* object =
                        static_cast<const nucleus::neutron::Object*>(&block);
                      assert(dynamic_cast<const nucleus::neutron::Object*>(
                               &block) != nullptr);

                      // validate the object according to the presence of
                      // a referenced access block.
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

                          // validate the object, providing the
                          if (object->Validate(address,
                                               *access) == elle::Status::Error)
                            throw reactor::Exception(elle::concurrency::scheduler(), "unable to validate the object");
                        }
                      else
                        {
                          // validate the object.
                          if (object->Validate(
                                address,
                                nucleus::neutron::Access::Null) == elle::Status::Error)
                            throw reactor::Exception(elle::concurrency::scheduler(), "unable to validate the object");
                        }

                      break;
                    }
                  default:
                    {
                      // validate the block through the common interface.
                      if (block.Validate(address) == elle::Status::Error)
                        throw reactor::Exception(elle::concurrency::scheduler(), "the block seems to be invalid");

                      break;
                    }
                  case nucleus::neutron::ComponentUnknown:
                    {
                      throw reactor::Exception(elle::concurrency::scheduler(),
                                               elle::sprintf("unknown component '%u'",
                                                             address.component));
                    }
                  }

                // store the block.
                if (block.Store(Hole::Implementation->network,
                                address) == elle::Status::Error)
                  throw reactor::Exception(elle::concurrency::scheduler(), "unable to store the block");
              }

              // Publish it onto the network.
              {
                for (auto neighbour: this->_hosts)
                  {
                    Host* host = neighbour.second;
                    host->push(address, block);
                    // ignore the error messages and continue with the
                    // next neighbour.
                    purge();
                  }
              }

              break;
            }
          default:
            {
              throw reactor::Exception
                (elle::concurrency::scheduler(),
                 elle::sprintf("the machine's state '%u' does not allow one to request "
                               "operations on the storage layer",
                               this->_state));
            }
          }
      }

      std::unique_ptr<nucleus::proton::Block>
      Machine::Get(const nucleus::proton::Address& address)
      {
        ELLE_LOG_TRACE("Get[Immutable]");

        // depending on the machine's state.
        switch (this->_state)
          {
            case State::attached:
            {
              //
              // in this case, the current machine is connected and has
              // been authenticated as a valid node of the network.
              //
              // therefore, the operation is carried out both locally but
              // also sent to every node in the network.
              //
              nucleus::proton::ImmutableBlock* block;
              nucleus::Nucleus::Factory.Build(address.component, block);

              // does the block exist.
              if (block->Exist(Hole::Implementation->network,
                              address) == elle::Status::True)
                {
                  // load the block.
                  if (block->Load(Hole::Implementation->network,
                                 address) == elle::Status::Error)
                    throw reactor::Exception(elle::concurrency::scheduler(), "unable to load the block");

                  // validate the block.
                  if (block->Validate(address) == elle::Status::Error)
                    throw reactor::Exception(elle::concurrency::scheduler(), "the block seems to be invalid");
                  return std::unique_ptr<nucleus::proton::Block>(block);
                }
              else
                {
                  // Otherwise, go through the neighbours and retrieve
                  // the block from them.
                  bool found = false;
                  for (auto neighbour: this->_hosts)
                    {
                      Host* host = neighbour.second;
                      std::unique_ptr<nucleus::proton::MutableBlock> block(
                        host->pull(address, nucleus::proton::Version::Any));

                      // validate the block.
                      if (block->Validate(address) == elle::Status::Ok)
                        {
                          // finally, store it locally.
                          if (block->Store(Hole::Implementation->network,
                                      address) == elle::Status::Ok)
                            found = true;
                            break;
                        }
                      else
                        {
                          // XXX
                          ELLE_LOG_TRACE("unable to validate the immutable block");
                          address.Dump();
                          block->Dump();
                          host->Dump();
                          show();
                          assert(false);
                        }
                    }

                  // Check if none if the neighbour has the block.
                  if (!found)
                    throw reactor::Exception(
                      elle::concurrency::scheduler(),
                      "unable to retrieve the block associated with "
                      "the given address from the other peers");

                  // Finally, load the block since it has been
                  // retrieved and stored locally.
                  if (block->Load(Hole::Implementation->network,
                                 address) == elle::Status::Error)
                    throw reactor::Exception(elle::concurrency::scheduler(),
                                             "unable to load the block");
                  return std::unique_ptr<nucleus::proton::Block>(block);
                }
            }
          default:
            {
              throw reactor::Exception
                (elle::concurrency::scheduler(),
                 elle::sprintf("the machine's state '%u' does not allow one to request "
                               "operations on the storage layer",
                               this->_state));
            }
          }
      }

      template <typename T>
      using Ptr = std::unique_ptr<T>;

      Ptr<nucleus::proton::Block>
      Machine::_get_latest(const nucleus::proton::Address&    address)
      {
        // Contact all the hosts in order to retrieve the latest
        // version of the block.
        //
        // This is required since no synchronisation mechanism is
        // present yet so the current machine may have missed some
        // versions when disconnected.
        using nucleus::proton::MutableBlock;
        using nucleus::proton::Version;
        using nucleus::neutron::Object;
        using nucleus::proton::Block;

        for (auto neighbour: this->_hosts)
          {
            Host* host = neighbour.second;
            Ptr<MutableBlock> block(host->pull(address, Version::Last));

            // Validate the block, depending on its component.
            // Indeed, the Object component requires as additional
            // block for being validated.
            switch (address.component)
              {
                case nucleus::neutron::ComponentObject:
                {
                  assert(dynamic_cast<Object const*>(block.get()) != nullptr);
                  Object const& object =
                    *static_cast<Object*>(block.get());

                  // Validate the object according to the presence of
                  // a referenced access block.
                  if (object.meta.access != nucleus::proton::Address::Null)
                    {
                      Ptr<Block> block(Hole::Pull(object.meta.access, Version::Last));
                      Ptr<nucleus::neutron::Access> access
                        (dynamic_cast<nucleus::neutron::Access*>(block.release()));
                      if (access == nullptr)
                        throw reactor::Exception(elle::concurrency::scheduler(),
                                                 "expected an access block");

                      // validate the object, providing the
                      if (object.Validate(address, *access)
                          == elle::Status::Error)
                        throw reactor::Exception(elle::concurrency::scheduler(),
                                                 "unable to validate the object");
                    }
                  else
                    {
                      // Validate the object.
                      if (object.Validate(
                            address,
                            nucleus::neutron::Access::Null) ==
                          elle::Status::Error)
                        throw reactor::Exception(elle::concurrency::scheduler(),
                                                 "unable to validate the object");
                    }

                  break;
                }
                default:
                {
                  // validate the block through the common
                  // interface.
                  if (block->Validate(address) == elle::Status::Error)
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

            // Finally, since the block has been retrieved,
            // store it locally.
            block->Store(Hole::Implementation->network, address);
            // XXX do not check the result as the block to
            // XXX store may not be the latest.

            // ignore the error messages and continue with the
            // next neighbour.
            purge();
          }

        // At this point, we may have retrieved one or more versions
        // of the mutable block but we do not have any guarantee.

        MutableBlock* block;
        nucleus::Nucleus::Factory.Build(address.component, block);

        // does the block exist.
        if (block->Exist(Hole::Implementation->network, address, Version::Last)
            == elle::Status::True)
          {
            // load the block.
            if (block->Load(Hole::Implementation->network, address, Version::Last)
                == elle::Status::Error)
              throw reactor::Exception(elle::concurrency::scheduler(),
                                       "unable to load the block");

            // Validate the block, depending on its component.
            // although every stored block has been checked, the block
            // may have been corrupt while on the hard disk.
            //
            // Note that the Object component requires as additional
            // block for being validated.
            switch (address.component)
              {
                case nucleus::neutron::ComponentObject:
                {
                  const Object* object =
                    static_cast<const Object*>(block);
                  assert(dynamic_cast<const Object*>(
                           block) != nullptr);
                  // Validate the object according to the presence of
                  // a referenced access block.
                  if (object->meta.access != nucleus::proton::Address::Null)
                    {
                      // Load the access block.
                      Ptr<Block> block
                        (Hole::Pull(object->meta.access, nucleus::proton::Version::Last));
                      Ptr<nucleus::neutron::Access> access
                        (dynamic_cast<nucleus::neutron::Access*>(block.release()));
                      if (access == nullptr)
                        throw reactor::Exception(elle::concurrency::scheduler(),
                                                 "expected an access block");
                      // Validate the object, providing the
                      if (object->Validate(address, *access) ==
                          elle::Status::Error)
                        throw reactor::Exception(elle::concurrency::scheduler(), "unable to validate the object");
                    }
                  else
                    {
                      // Validate the object.
                      if (object->Validate(
                            address,
                            nucleus::neutron::Access::Null) ==
                          elle::Status::Error)
                        throw reactor::Exception(elle::concurrency::scheduler(), "unable to validate the object");
                    }

                  break;
                }
                default:
                {
                  // validate the block through the common interface.
                  if (block->Validate(address) == elle::Status::Error)
                    throw reactor::Exception(elle::concurrency::scheduler(), "the block seems to be invalid");

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
        return Ptr<Block>(block);
      }


      std::unique_ptr<nucleus::proton::Block>
      Machine::_get_specific(const nucleus::proton::Address& address,
                             nucleus::proton::Version const& version)
      {
        // Go through the neighbours and retrieve the
        // specific version of the block.

        using nucleus::neutron::Object;
        using nucleus::proton::MutableBlock;

        Ptr<MutableBlock> block;
        {
          MutableBlock* raw;
          nucleus::Nucleus::Factory.Build(address.component, raw);
          block = Ptr<MutableBlock>(raw);
        }

        // Does the block exist: if it does not, retrieve it from the
        // peers.
        if (block->Exist(Hole::Implementation->network, address, version)
            == elle::Status::False)
          {
            bool found = false;
            for (auto neighbour: this->_hosts)
              {
                Host*                     host = neighbour.second;
                nucleus::Derivable derivable;

                // request the host.
                Ptr<nucleus::proton::MutableBlock> block(
                  host->pull(address, version));

                // validate the block, depending on its
                // component.
                //
                // indeed, the Object component requires as
                // additional block for being validated.
                switch (address.component)
                  {
                    case nucleus::neutron::ComponentObject:
                    {
                      Object const&  object =
                        static_cast<Object const&>(
                          derivable.block());
                      assert(dynamic_cast<Object const*>(
                               &(derivable.block())) != nullptr);

                      // validate the object according to the
                      // presence of a referenced access block.
                      if (object.meta.access !=
                          nucleus::proton::Address::Null)
                        {
                          Ptr<nucleus::proton::Block> block
                            (Hole::Pull(object.meta.access, nucleus::proton::Version::Last));
                          Ptr<nucleus::neutron::Access> access
                            (dynamic_cast<nucleus::neutron::Access*>(block.release()));
                          if (access == nullptr)
                            throw reactor::Exception(elle::concurrency::scheduler(),
                                                     "expected an access block");

                          // validate the object, providing the
                          if (object.Validate(
                                address,
                                *access) == elle::Status::Error)
                            throw reactor::Exception(elle::concurrency::scheduler(),
                                                     "unable to validate the "
                                                     "object");
                        }
                      else
                        {
                          // validate the object.
                          if (object.Validate(
                                address,
                                nucleus::neutron::Access::Null) ==
                              elle::Status::Error)
                            throw reactor::Exception(elle::concurrency::scheduler(),
                                                     "unable to validate the "
                                                     "object");
                        }
                      found = true;
                      break;
                    }
                    default:
                    {
                      // Validate the block through the common
                      // interface.
                      if (derivable.block().Validate(address) ==
                          elle::Status::Error)
                        throw reactor::Exception(elle::concurrency::scheduler(), "the block seems to be invalid");
                    }
                    case nucleus::neutron::ComponentUnknown:
                    {
                      throw reactor::Exception(elle::concurrency::scheduler(),
                                               elle::sprintf("unknown component '%u'",
                                                             address.component));
                    }
                  }

                // Finally, since the block has been retrieved,
                // store it locally.
                block->Store(Hole::Implementation->network, address);
                // XXX do not check the result as the block to
                // XXX store may not be the latest i.e when
                // XXX history is not active.

                // stop since a block for this specific
                // version has been retrieved.
                break;

                // ignore the error messages and continue with the
                // next neighbour.
                purge();
              }

            // check if none if the neighbour has the block.
            if (!found)
              throw reactor::Exception(elle::concurrency::scheduler(),
                                       "unable to retrieve the block associated with "
                                       "the given address from the other peers");
          }

        // Now let us try to retrieve the block from the local
        // storage.

        // Does the block exist.
        if (block->Exist(Hole::Implementation->network,
                         address,
                         version) == elle::Status::True)
          {
            // Load the block.
            if (block->Load(Hole::Implementation->network,
                            address,
                            version) == elle::Status::Error)
              throw reactor::Exception(elle::concurrency::scheduler(), "unable to load the block");

            // validate the block, depending on its component.
            // although every stored block has been checked, the
            // block may have been corrupt while on the hard disk.
            //
            // note that the Object component requires as additional
            // block for being validated.
            switch (address.component)
              {
                case nucleus::neutron::ComponentObject:
                {
                  assert(dynamic_cast<const Object*>(block.get()) != nullptr);
                  const Object* object = static_cast<const Object*>(block.get());

                  // validate the object according to the presence of
                  // a referenced access block.
                  if (object->meta.access !=
                      nucleus::proton::Address::Null)
                    {
                      // Load the access block.
                      Ptr<nucleus::proton::Block> block
                        (Hole::Pull(object->meta.access, nucleus::proton::Version::Last));
                      Ptr<nucleus::neutron::Access> access
                        (dynamic_cast<nucleus::neutron::Access*>(block.release()));
                      if (access == nullptr)
                        throw reactor::Exception(elle::concurrency::scheduler(),
                                                 "expected an access block");

                      // Validate the object.
                      if (object->Validate(address, *access) == elle::Status::Error)
                        throw reactor::Exception(elle::concurrency::scheduler(), "unable to validate the object");
                    }
                  else
                    {
                      // validate the object.
                      if (object->Validate(
                            address,
                            nucleus::neutron::Access::Null) ==
                          elle::Status::Error)
                        throw reactor::Exception(elle::concurrency::scheduler(), "unable to validate the object");
                    }

                  break;
                }
                default:
                {
                  // validate the block through the common interface.
                  if (block->Validate(address) == elle::Status::Error)
                    throw reactor::Exception(elle::concurrency::scheduler(), "the block seems to be invalid");

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
        return std::move(block);
      }

      std::unique_ptr<nucleus::proton::Block>
      Machine::Get(const nucleus::proton::Address&    address,
                   const nucleus::proton::Version&    version)
      {
        ELLE_LOG_TRACE("Get[Mutable]");

        // Check the machine is connected and has been authenticated
        // as a valid node of the network.
        if (this->_state != State::attached)
          throw reactor::Exception(\
            elle::concurrency::scheduler(),
            elle::sprintf("the machine's state '%u' does not allow one "
                          "to request operations on the storage layer",
                          this->_state));

        if (version == nucleus::proton::Version::Last)
          return _get_latest(address);
        else
          return _get_specific(address, version);
      }

      void
      Machine::Kill(const nucleus::proton::Address&   address)
      {
        ELLE_LOG_TRACE("Kill");

        // depending on the machine's state.
        switch (this->_state)
          {
            case State::attached:
            {
              //
              // in this case, the current machine is connected and has
              // been authenticated as a valid node of the network.
              //
              // therefore, the operation is carried out both locally but
              // also sent to every node in the network.
              //

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
                        throw reactor::Exception(elle::concurrency::scheduler(), "unable to erase the block");

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
                        throw reactor::Exception(elle::concurrency::scheduler(), "unable to erase the block");

                      break;
                    }
                  default:
                    {
                      throw reactor::Exception(elle::concurrency::scheduler(), "unknown block family");
                    }
                  }
              }

              // Notify the other hosts of the removal.
              {
                // for every scoutor.
                for (auto neighbour: this->_hosts)
                  {
                    Host* host = neighbour.second;
                    host->wipe(address);
                    // Ignore the error messages and continue with the
                    // next neighbour.
                    purge();
                  }
              }

              break;
            }
          default:
            {
              throw reactor::Exception(elle::concurrency::scheduler(),
                                       elle::sprintf("the machine's state '%u' does not allow one to "
                                                     "request operations on the storage layer",
                                                     this->_state));
            }
          }
      }

      void
      Machine::_alone()
      {
        ELLE_LOG_TRACE("alone");

        // If the machine has been neither connected nor authenticated
        // to existing nodes...
        if (this->_state == State::detached)
          {
            // Then, suppose that the current machine as the only one
            // in the network.  Thus, it can be implicitly considered
            // as authenticated in a network composed of itself alone.
            this->_state = State::attached;

            // Set the hole as ready to receive requests.
            if (Hole::Ready() == elle::Status::Error)
              throw reactor::Exception(elle::concurrency::scheduler(),
                                       "unable to set the hole online");
          }
      }

      /*---------.
      | Dumpable |
      `---------*/

      std::ostream&
      operator << (std::ostream& stream, Machine::State state)
      {
        switch (state)
          {
            case Machine::State::detached:
              stream << "detached";
              break;
            case Machine::State::attached:
              stream << "attached";
              break;
          }
        return stream;
      }

      elle::Status
      Machine::Dump(const elle::Natural32 margin) const
      {
        elle::String    alignment(margin, ' ');

        std::cout << alignment << "[Machine]" << std::endl;
        std::cout << alignment << elle::io::Dumpable::Shift
                  << "[State] " << this->_state << std::endl;
        std::cout << alignment << elle::io::Dumpable::Shift
                  << "[Port] " << std::dec << this->_port << std::endl;
        return elle::Status::Ok;
      }
    }
  }
}
