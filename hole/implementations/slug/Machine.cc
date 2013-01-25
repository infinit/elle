#include <reactor/network/exception.hh>
#include <reactor/network/udt-server.hh>

// FIXME
#include <common/common.hh>
#include <elle/io/Piece.hh>
#include <lune/Lune.hh>
#include <lune/Descriptor.hh>
#include <plasma/meta/Client.hh>

#include <elle/cast.hh>
#include <elle/Exception.hh>
#include <elle/log.hh>
#include <elle/network/Interface.hh>
#include <elle/utility/Time.hh>
#include <elle/utility/Duration.hh>

#include <agent/Agent.hh>

#include <hole/Hole.hh>
#include <hole/implementations/slug/Host.hh>
#include <hole/implementations/slug/Implementation.hh>
#include <hole/implementations/slug/Machine.hh>
#include <hole/implementations/slug/Manifest.hh>

#include <elle/Passport.hh>

#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Block.hh>
#include <nucleus/proton/Revision.hh>
#include <nucleus/proton/ImmutableBlock.hh>
#include <nucleus/proton/MutableBlock.hh>
#include <nucleus/neutron/Object.hh>
#include <nucleus/neutron/Access.hh>
#include <nucleus/Derivable.hh>

#include <Infinit.hh>

#include <algorithm>

ELLE_LOG_COMPONENT("infinit.hole.slug.Machine");

// XXX[to improve later with a configuration variable]
#define CACHE
#define CACHE_LIFESPAN 30 /* in seconds */

namespace hole
{
  namespace implementations
  {
    namespace slug
    {
      // FIXME
      static Machine* machine(nullptr);
      void
      portal_connect(std::string const& host, int port)
      {
        machine->portal_connect(host, port);
      }

      void
      Machine::portal_connect(std::string const& host, int port)
      {
        ELLE_TRACE_FUNCTION(host, port);

        _server->accept(host, port);
      }

      bool
      portal_wait(std::string const& host, int port)
      {
        return (machine->portal_wait(host, port));
      }

      reactor::Signal _machine_wait("machine wait");
      reactor::Signal _host_wait("host wait");

      // XXX[very simple version where we assume there is a single node to
      //     which we will connect. otherwise, one would need to loop until...]
      bool
      Machine::portal_wait(std::string const& host, int port)
      {
        ELLE_TRACE_FUNCTION(host, port);

        elle::network::Locus locus(host, port);

        ELLE_TRACE("checking if the host '%s' is present and has been "
                   "authenticated", locus);

        // Look for the given host in the list of hosts and
        // make sure it has been authenticated.
        auto i = this->_hosts.find(locus);
        if ((i == this->_hosts.end()) ||
            (i->second->state() != Host::State::authenticated))
          {
            ELLE_TRACE("waiting for the host '%s' to authenticate",
                       locus);

            // Wait for a new host to be authenticated.
            elle::concurrency::scheduler().current()->wait(
              _host_wait,
              boost::posix_time::seconds(10));

            ELLE_TRACE("a host seems to have been authenticated, check again");

            // Recheck and return an error if it's still not authenticated.
            i = this->_hosts.find(locus);
            if ((i == this->_hosts.end()) ||
                (i->second->state() != Host::State::authenticated))
              {
                ELLE_TRACE("the host '%s' still has not been authenticated, "
                           "abandon", locus);

                return (false);
              }
          }

        ELLE_TRACE("now checking if the machine has also been authenticated "
                   "by the host");

        // Look for the given host in the list of hosts and
        // make sure the machine has been able to authenticate to the host.
        auto j = this->_hosts.find(locus);
        ELLE_ASSERT(j != this->_hosts.end());
        if (j->second->authenticated() == false)
          {
            ELLE_TRACE("waiting for the machine to authenticate to host '%s'",
                       locus);

            // Wait for the machine to be authenticated by the host as well.
            elle::concurrency::scheduler().current()->wait(
              _machine_wait,
              boost::posix_time::seconds(10));

            ELLE_TRACE("the machine seems to have been authenticated to a host, "
                       "check again");

            // Recheck and return an error if it's still not authenticated.
            auto j = this->_hosts.find(locus);
            ELLE_ASSERT(j != this->_hosts.end());
            if (j->second->authenticated() == false)
              {
                ELLE_TRACE("the machine still has not authenticate to "
                           "the host '%s', abandon", locus);

                return (false);
              }
          }

        ELLE_TRACE("both the machine and the host have been authenticated");

        return (true);
      }

      void
      portal_machine_authenticated()
      {
        _machine_wait.signal();
      }

      void
      portal_host_authenticated()
      {
        _host_wait.signal();
      }

      // FIXME

      /*----------.
      | Variables |
      `----------*/

#ifdef CACHE
      static std::map<elle::String, elle::utility::Time> cache;
#endif

      /*-------------.
      | Construction |
      `-------------*/

      void
      Machine::_connect(elle::network::Locus const& locus)
      {
        ELLE_TRACE_SCOPE("try connecting to %s", locus);
        std::string hostname;
        locus.host.Convert(hostname);
        std::unique_ptr<reactor::network::Socket> socket(
          reactor::network::Socket::create(
            this->hole().protocol(),
            elle::concurrency::scheduler(),
            hostname, locus.port, _connection_timeout));
        _connect(std::move(socket), locus, true);
      }

      void
      Machine::_connect(std::unique_ptr<reactor::network::Socket> socket,
                        elle::network::Locus const& locus, bool opener)
      {
        std::unique_ptr<Host> host(new Host(*this, locus, std::move(socket)));
        ELLE_TRACE("%s: authenticate to host: %s", *this, locus);
        auto loci = host->authenticate(this->_hole.passport());
        if (this->_state == State::detached)
          {
            this->_state = State::attached;
            this->_hole.ready();
          }
        for (auto locus: loci)
          if (_hosts.find(locus) == _hosts.end())
            _connect_try(locus);
        ELLE_LOG("%s: add host: %s", *this, *host);
        _hosts[locus] = host.release();
      }

      void
      Machine::_connect_try(elle::network::Locus const& locus)
      {
        try
          {
            _connect(locus);
          }
        catch (reactor::network::Exception& err)
          {
            ELLE_TRACE("ignore host %s: %s", locus, err.what());
          }
      }

      void
      Machine::_remove(Host* host)
      {
        elle::network::Locus locus(host->locus());
        assert(this->_hosts.find(locus) != this->_hosts.end());
        ELLE_LOG("%s: remove host: %s", *this, *host);
        this->_hosts.erase(locus);
        delete host;
      }

      Machine::Machine(Implementation& hole,
                       int port,
                       reactor::Duration connection_timeout)
        : _hole(hole)
        , _connection_timeout(connection_timeout)
        , _state(State::detached)
        , _port(port)
        , _server(reactor::network::Server::create
                  (hole.protocol(), elle::concurrency::scheduler()))
        , _acceptor()
      {
        machine = this; // FIXME
        elle::network::Locus     locus;
        ELLE_TRACE_SCOPE("launch");

        // Connect to hosts from the descriptor.
        {
          // FIXME: use builtin support for subcoroutines when available.
          std::vector<reactor::Thread*> connections;
          auto& sched = elle::concurrency::scheduler();
          for (elle::network::Locus const& locus: this->hole().members())
            {
              auto action = [&, locus] {this->_connect_try(locus);};
              auto thread = new reactor::Thread
                (sched, elle::sprintf("connect %s", locus), action);
              connections.push_back(thread);
            }
          for (reactor::Thread* t: connections)
            {
              sched.current()->wait(*t);
              delete t;
            }
        }

        // If the machine has been neither connected nor authenticated
        // to existing nodes...
        if (this->_state == State::detached)
          {
            // Then, suppose that the current machine as the only one
            // in the network.  Thus, it can be implicitly considered
            // as authenticated in a network composed of itself alone.
            this->_state = State::attached;

            // Set the hole as ready to receive requests.
            this->_hole.ready();
          }

        // Finally, listen for incoming connections.
        {
          elle::network::Locus locus;
          elle::network::Host host(elle::network::Host::TypeAny);
          try
            {
              _server->listen(this->_port);
              // In case we asked for a random port to be picked up (by using 0)
              // or hole punching happened, retrieve the actual listening port.
              this->_port = _server->port();
              ELLE_ASSERT(this->_port != 0);
              ELLE_TRACE("listening on port %s", this->_port);
              _acceptor.reset(new reactor::Thread(elle::concurrency::scheduler(),
                                                  "Slug accept",
                                                  [&] {this->_accept();}));
            }
          catch (reactor::Exception& e)
            {
              ELLE_ERR("unable to accept incoming connections: %s", e.what());
              // FIXME: what do ? For now, just go on without
              // listening. Useful when testing with several clients
              // on the same machine.
            }
        }


        // Send addresses to meta.
        {
          lune::Descriptor descriptor(Infinit::User, Infinit::Network);
          plasma::meta::Client client(common::meta::host(), common::meta::port());
          try
            {
              elle::io::Path passport_path(lune::Lune::Passport);
              passport_path.Complete(elle::io::Piece{"%USER%", Infinit::User});
              elle::Passport passport;
              passport.load(passport_path);

              std::vector<std::pair<std::string, uint16_t>> addresses;
              auto interfaces = elle::network::Interface::get_map(
                elle::network::Interface::Filter::only_up
                | elle::network::Interface::Filter::no_loopback
                );
              for (auto const& pair: interfaces)
                if (pair.second.ipv4_address.size() > 0 &&
                    pair.second.mac_address.size() > 0)
                  {
                    ELLE_DEBUG("local address: %s:%s",
                               pair.second.ipv4_address,
                               _server->port());

                    addresses.emplace_back(pair.second.ipv4_address,
                                           _server->port());
                  }

              std::vector<std::pair<std::string, uint16_t>> public_addresses;
              auto udt = dynamic_cast<reactor::network::UDTServer*>
                (_server.get());
              assert(udt);
              auto ep = udt->public_endpoint();
              public_addresses.push_back(std::pair<std::string, uint16_t>
                                         (ep.address().to_string(),
                                          ep.port()));
              client.token(agent::Agent::meta_token);

              ELLE_DEBUG("addresses");
              std::for_each(addresses.begin(), addresses.end(),
                            [](std::pair<std::string, uint16_t> const& e)
                            { ELLE_DEBUG("%s:%s", e.first, e.second); });

              ELLE_DEBUG("public_addresses");
              std::for_each(public_addresses.begin(), public_addresses.end(),
                            [](std::pair<std::string, uint16_t> const& e)
                            { ELLE_DEBUG("%s:%s", e.first, e.second); });

              client.network_connect_device(descriptor.meta().id(),
                                            passport.id(),
                                            addresses,
                                            public_addresses);
            }
          catch (std::exception const& err)
            {
              ELLE_ERR("Cannot update device port: %s",
                       err.what()); // XXX[to improve]
            }
        }
      }

      Machine::~Machine()
      {
        for (auto host: Hosts(_hosts))
          this->_remove(host.second);

        // Stop serving; we may not be listening, since bind errors are
        // considered warnings (see constructor), in which case we have no
        // acceptor.
        if (_acceptor)
          _acceptor->terminate_now();
      }

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
            std::unique_ptr<reactor::network::Socket> socket(_server->accept());

            ELLE_TRACE_SCOPE("accept connection from %s",
                             socket->remote_locus());

#ifdef CACHE
            {
              // We need to clear cached blocks whenever a node joins the
              // network.
              //
              // Indeed, assuming the block B is in cache at revision 4
              // and considering the new node actually as a newer revision
              // of the block, say 5.
              //
              // By clearing the cache, the system will make sure to ask
              // the peers for the latest revision of the block. Without it,
              // the block revision 5 would still be used.
              //
              // @see hole::backends::fs::MutableBlock::derives()
              // XXX this should be done once the host is authenticated.
              ELLE_LOG_COMPONENT("infinit.hole.slug.cache");
              ELLE_TRACE("cleaning the cache");
              cache.clear();
            }
#endif

            // Depending on the machine's state.
            switch (this->_state)
              {
                case State::attached:
                {
                  // FIXME: handling via loci is very wrong. IPs are
                  // not uniques, and this reconstruction is lame and
                  // non-injective.
                  auto locus = socket->remote_locus();
                  _connect(std::move(socket), locus, false);
                  break;
                }
                default:
                {
                  // FIXME: Why not listening only when we're attached ?
                  ELLE_TRACE("not attached, ignore %s", socket->remote_locus());
                  break;
                }
              }
          }
      }

      /*----.
      | API |
      `----*/

      void
      Machine::put(const nucleus::proton::Address& address,
                   const nucleus::proton::ImmutableBlock& block)
      {
        ELLE_TRACE_METHOD(address, block);

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
                block.validate(address);

                this->_hole.storage().store(address, block);
              }

              // Publish it onto the network.
              {
                for (auto neighbour: _hosts)
                  {
                    Host* host = neighbour.second;

                    try
                      {
                        host->push(address, block);
                      }
                    catch (std::exception const& e)
                      {
                        ELLE_WARN("[%p] remote exception: %s",
                                  this, e.what());
                        continue;
                      }
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
      Machine::put(const nucleus::proton::Address& address,
                   const nucleus::proton::MutableBlock& block)
      {
        ELLE_TRACE_METHOD(address, block);

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
                switch (address.component())
                  {
                  case nucleus::neutron::ComponentObject:
                    {
                      /* XXX[need to change the way validation works by relying
                             on a callback]
                      const nucleus::neutron::Object* object =
                        static_cast<const nucleus::neutron::Object*>(&block);
                      assert(dynamic_cast<const nucleus::neutron::Object*>(
                               &block) != nullptr);

                      // validate the object according to the presence of
                      // a referenced access block.
                      if (object->access() != nucleus::proton::Address::null())
                        {
                          // Load the access block.
                          std::unique_ptr<nucleus::proton::Block> addressBlock
                            (this->_hole.pull(object->access(),
                                                   nucleus::proton::Revision::Last));

                          // Get access block.
                          nucleus::neutron::Access * access =
                            dynamic_cast<nucleus::neutron::Access *>(addressBlock.get());

                          if (access == nullptr)
                            throw reactor::Exception(elle::concurrency::scheduler(),
                                                     "expected an access block");

                          // validate the object, providing the
                          object->validate(address, access);
                        }
                      else
                        {
                          // validate the object.
                          object->validate(address, nullptr);
                        }
                      */

                      break;
                    }
                  default:
                    {
                      // validate the block through the common interface.
                      block.validate(address);

                      break;
                    }
                  case nucleus::neutron::ComponentUnknown:
                    {
                      throw reactor::Exception(elle::concurrency::scheduler(),
                                               elle::sprintf("unknown component '%u'",
                                                             address.component()));
                    }
                  }

                this->_hole.storage().store(address, block);
              }

              // Publish it onto the network.
              {
                for (auto neighbour: this->_hosts)
                  {
                    Host* host = neighbour.second;

                    try
                      {
                        host->push(address, block);
                      }
                    catch (std::exception const& e)
                      {
                        ELLE_WARN("[%p] remote exception: %s",
                                  this, e.what());
                        continue;
                      }
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

#ifdef CACHE
        {
          ELLE_LOG_COMPONENT("infinit.hole.slug.cache");

          // Finally, now that the block has been accepted as a valid revision
          // of the mutable block, record it in the cache so that the machine
          // will no longer have to fetch the block from the other peers since
          // it already has the last revision and the other nodes will publish
          // any new revision.
          elle::String unique = address.unique();
          auto iterator = cache.find(unique);

          if (iterator == cache.end())
            {
              elle::String unique = address.unique();

              ELLE_TRACE("%s: register %s", *this, unique);

              elle::utility::Time current;

              if (current.Current() == elle::Status::Error)
                throw reactor::Exception(elle::concurrency::scheduler(),
                                         "unable to retrieve the current time");

              auto result =
                cache.insert(std::pair<elle::String,
                                       elle::utility::Time>(unique, current));

              if (result.second == false)
                throw reactor::Exception(
                  elle::concurrency::scheduler(),
                  elle::sprintf("unable to insert the address '%s' in the cache",
                                unique));
            }
          else
            {
              elle::utility::Time current;

              ELLE_TRACE("%s: update %s", *this, unique);

              if (current.Current() == elle::Status::Error)
                throw reactor::Exception(elle::concurrency::scheduler(),
                                         "unable to retrieve the current time");

              iterator->second = current;
            }
        }
#endif
      }

      std::unique_ptr<nucleus::proton::Block>
      Machine::get(const nucleus::proton::Address& address)
      {
        ELLE_TRACE_METHOD(address);

        using nucleus::proton::ImmutableBlock;

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

              std::unique_ptr<nucleus::proton::Block> block;

              // does the block exist.
              if (!this->_hole.storage().exist(address))
                {
                  ELLE_TRACE("the immutable block does not exist locally,"
                             " fetch %s from the peers", address);
                  // Go through the neighbours and retrieve
                  // the block from them.
                  bool found = false;
                  for (auto neighbour: this->_hosts)
                    {
                      Host* host = neighbour.second;
                      assert(host != nullptr);

                      std::unique_ptr<nucleus::proton::ImmutableBlock> iblock;
                      ELLE_TRACE("fetch %s from peer %s", address, *host);

                      try
                        {
                          iblock = elle::cast<ImmutableBlock>::runtime(
                              host->pull(address, nucleus::proton::Revision::Any));
                        }
                      catch (std::exception const& e)
                        {
                          ELLE_WARN("%s: remote exception: %s",
                                    this, e.what());
                          continue;
                        }

                      ELLE_TRACE("block fetched: %s", *iblock);

                      // validate the block.
                      try
                        {
                          this->_hole.storage().store(address, *iblock);

                          found = true;

                          // No need to continue since the block is immutable.
                          break;
                        }
                      catch (nucleus::Exception const& e)
                        {
                          ELLE_WARN("%s: unable to validate the block '%s'",
                                    this, address);
                          continue;
                        }
                    }

                  // Check if none if the neighbour has the block.
                  if (!found)
                    throw reactor::Exception(
                      elle::concurrency::scheduler(),
                      "unable to retrieve the block associated with "
                      "the given address from the other peers");
                }

              assert(this->_hole.storage().exist(address));

              ELLE_TRACE("load the local block at %s", address)
              {
                // load the block.
                block = this->_hole.storage().load(address);
              }

              // validate the block.
              block->validate(address);

              ELLE_TRACE("block loaded and validated: %s", *block);

              return block;
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
        ELLE_DEBUG_METHOD(address);

        // Contact all the hosts in order to retrieve the latest
        // revision of the block.
        //
        // This is required since no synchronisation mechanism is
        // present yet so the current machine may have missed some
        // revisions when disconnected.
        using nucleus::proton::MutableBlock;
        using nucleus::proton::Revision;
        using nucleus::neutron::Object;
        using nucleus::proton::Block;

#ifdef CACHE
        elle::String unique = address.unique();
        auto iterator = cache.find(unique);
        {
          ELLE_LOG_COMPONENT("infinit.hole.slug.cache");

          // First, check whether the address is already contained in the
          // cache. If so, the local block is used rather than issuing network
          // communication. The goal of this cache is to say 'the local revision
          // of the block is the latest, so use it instead of bothering everyone
          // else'. Note that this optimization works because all nodes are
          // supposed to be trustworthy (in the 'slug' implementation) and
          // therefore to send the newest revision of every modified block.

          if (iterator != cache.end())
            {
              elle::utility::Time deadline =
                iterator->second +
                elle::utility::Duration(elle::utility::Duration::UnitSeconds,
                                        CACHE_LIFESPAN);
              elle::utility::Time current;

              if (current.Current() == elle::Status::Error)
                throw reactor::Exception(elle::concurrency::scheduler(),
                                         "unable to retrieve the current time");

              if (current < deadline)
                {
                  // In this case, the block is still valid and can therefore
                  // be used.
                  Ptr<nucleus::proton::Block> ptr;

                  // Make sure the block exists, otherwise, fall down to the
                  // usual case: retrieving the block from the network.
                  if (this->_hole.storage().exist(address))
                    {
                      ELLE_DEBUG("%s: cache hit on %s", *this, unique);

                      // Load the block.
                      ptr = this->_hole.storage().load(address);

                      return ptr;
                    }
                  else
                    {
                      ELLE_DEBUG("%s: cache hit but block %s not present locally", *this, unique);
                    }
                }
              else
                {
                  // Otherwise, the block must be discarded, as too old to
                  // be used.
                  //
                  // And the process must fall back to the original one which
                  // consists in retrieving the block from peers.

                  ELLE_DEBUG("%s: cache miss (expired) on %s", *this, unique);

                  cache.erase(iterator);
                }
            }
        }
#endif

        ELLE_DEBUG_SCOPE("%s: retrieving the block '%s' from the network",
                         this, address);

        for (auto neighbour: this->_hosts)
          {
            Host* host = neighbour.second;
            std::unique_ptr<MutableBlock> block;

            try
              {
                block =
                  elle::cast<MutableBlock>::runtime(
                    host->pull(address, Revision::Last));
              }
            catch (std::exception const& e)
              {
                ELLE_WARN("%s: remote exception: %s",
                          this, e.what());
                continue;
              }

            // Validate the block, depending on its component.
            // Indeed, the Object component requires as additional
            // block for being validated.
            switch (address.component())
              {
              case nucleus::neutron::ComponentObject:
                {
                  /* XXX[need to change the way validation works by relying
                         on a callback]
                  assert(dynamic_cast<Object const*>(block.get()) != nullptr);
                  Object const& object = *static_cast<Object*>(block.get());

                  // Validate the object according to the presence of
                  // a referenced access block.
                  if (object.access() != nucleus::proton::Address::null())
                    {
                      Ptr<Block> block(this->_hole.pull(object.access(), Revision::Last));
                      Ptr<nucleus::neutron::Access> access
                        (dynamic_cast<nucleus::neutron::Access*>(block.release()));
                      if (access == nullptr)
                        {
                          ELLE_WARN("%s: unable to load the access block for "
                                    "validating the object", this);
                          continue;
                        }

                      // validate the object, providing the
                      try
                        {
                          object.validate(address, access.get());
                        }
                      catch (nucleus::Exception const& e)
                        {
                          ELLE_WARN("%s: unable to validate the access block",
                                    this);
                          continue;
                        }
                    }
                  else
                    {
                      // Validate the object.
                      try
                        {
                          object.validate(address, nullptr);
                        }
                      catch (nucleus::Exception const& e)
                        {
                          ELLE_WARN("%s: unable to validate the access block",
                                    this);
                          continue;
                        }
                    }
                  */

                  break;
                }
              default:
                {
                  // validate the block through the common
                  // interface.
                  try
                    {
                      block->validate(address);
                    }
                  catch (nucleus::Exception const& e)
                    {
                      ELLE_WARN("%s: unable to validate the block",
                                this);
                      continue;
                    }

                  break;
                }
              case nucleus::neutron::ComponentUnknown:
                {
                  throw reactor::Exception(elle::concurrency::scheduler(),
                                           elle::sprintf("unknown component '%u'",
                                                         address.component()));
                }
              }

            // XXX It force conflict to be public. Can we change that ?
            ELLE_DEBUG("Check if the block derives the current block")
              if (this->_hole.storage().conflict(address, *block))
                {
                  ELLE_DEBUG("the block %p does not derive the local one",
                             block);
                  continue;
                }

            ELLE_DEBUG("storing the remote block %s locally", address)
              this->_hole.storage().store(address, *block);
          }

        // At this point, we may have retrieved one or more revisions
        // of the mutable block but we do not have any guarantee.

        if (!this->_hole.storage().exist(address))
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "unable to retrieve the mutable block");

        Ptr<MutableBlock> block;

        // load the block.
        ELLE_DEBUG("loading the local block at %s", address);

        block = elle::cast<MutableBlock>::runtime(
          this->_hole.storage().load(address));

        ELLE_DEBUG("loaded block %s has revision %s",
                   block, block->revision());

        ELLE_DEBUG("validating the block")
        // Validate the block, depending on its component.
        // although every stored block has been checked, the block
        // may have been corrupt while on the hard disk.
        //
        // Note that the Object component requires as additional
        // block for being validated.
        switch (address.component())
          {
          case nucleus::neutron::ComponentObject:
            {
              /* XXX[need to change the way validation works by relying
                     on a callback]
              const Object* object =
                static_cast<const Object*>(block.get());
              assert(dynamic_cast<const Object*>(block.get()) != nullptr);

              // Validate the object according to the presence of
              // a referenced access block.
              if (object->access() != nucleus::proton::Address::null())
                {
                  // Load the access block.
                  Ptr<Block> block
                    (this->_hole.pull(object->access(), nucleus::proton::Revision::Last));
                  Ptr<nucleus::neutron::Access> access
                    (dynamic_cast<nucleus::neutron::Access*>(block.release()));
                  if (access == nullptr)
                    throw reactor::Exception(elle::concurrency::scheduler(),
                                             "expected an access block");
                  // Validate the object, providing the
                  object->validate(address, access.get());
                }
              else
                {
                  // Validate the object.
                  object->validate(address, nullptr);
                }
              */

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

#ifdef CACHE
        {
          ELLE_LOG_COMPONENT("infinit.hole.slug.cache");

          // At this point, now that the block has been retrieved from
          // the network, the cache is updated by marking this mutable
          // block as up-to-date locally, meaning that the machine no
          // longer needs to fetch it from the other peers. Instead
          // the peers will notify everyone of the updated revisions.
          iterator = cache.find(unique);

          if (iterator == cache.end())
            {
              elle::String unique = address.unique();

              ELLE_DEBUG("%s: register %s", *this, unique);

              elle::utility::Time current;

              if (current.Current() == elle::Status::Error)
                throw reactor::Exception(elle::concurrency::scheduler(),
                                         "unable to retrieve the current time");

              auto result =
                cache.insert(std::pair<elle::String,
                                       elle::utility::Time>(unique, current));

              if (result.second == false)
                throw reactor::Exception(
                  elle::concurrency::scheduler(),
                  elle::sprintf("unable to insert the address '%s' in the cache",
                                unique));
            }
          else
            {
              elle::utility::Time current;

              ELLE_DEBUG("%s: update %s", *this, unique);

              if (current.Current() == elle::Status::Error)
                throw reactor::Exception(elle::concurrency::scheduler(),
                                         "unable to retrieve the current time");

              iterator->second = current;
            }
        }
#endif

        return Ptr<Block>(block.release());
      }


      std::unique_ptr<nucleus::proton::Block>
      Machine::_get_specific(const nucleus::proton::Address& address,
                             nucleus::proton::Revision const& revision)
      {
        // Go through the neighbours and retrieve the
        // specific revision of the block.

        using nucleus::neutron::Object;
        using nucleus::proton::MutableBlock;

        Ptr<MutableBlock> block;

        // If the block does not exist, retrieve it from the peers.
        if (!this->_hole.storage().exist(address, revision))
          {
            bool found = false;
            for (auto neighbour: this->_hosts)
              {
                Host* host = neighbour.second;
                nucleus::Derivable derivable;
                std::unique_ptr<MutableBlock> block;

                try
                  {
                    block =
                      elle::cast<MutableBlock>::runtime(
                        host->pull(address, revision));
                  }
                catch (std::exception const& e)
                  {
                    ELLE_WARN("%s: remote exception: %s",
                              this, e.what());
                    continue;
                  }

                // validate the block, depending on its
                // component.
                //
                // indeed, the Object component requires as
                // additional block for being validated.
                switch (address.component())
                  {
                  case nucleus::neutron::ComponentObject:
                    {
                      /* XXX[need to change the way validation works by relying
                             on a callback]
                      Object const&  object =
                        static_cast<Object const&>(derivable.block());
                      assert(dynamic_cast<Object const*>(
                               &(derivable.block())) != nullptr);

                      // validate the object according to the
                      // presence of a referenced access block.
                      if (object.access() != nucleus::proton::Address::null())
                        {
                          Ptr<nucleus::proton::Block> block
                            (this->_hole.pull(object.access(),
                                                   nucleus::proton::Revision::Last));
                          Ptr<nucleus::neutron::Access> access
                            (dynamic_cast<nucleus::neutron::Access*>(block.release()));
                          if (access == nullptr)
                            {
                              ELLE_WARN("%s: unable to retrieve the access "
                                        "block for validating the object",
                                        this);
                              continue;
                            }

                          // validate the object, providing the
                          try
                            {
                              object.validate(address, access.get());
                            }
                          catch (nucleus::Exception const& e)
                            {
                              ELLE_WARN("%s: unable to validate the object",
                                        this);
                              continue;
                            }
                        }
                      else
                        {
                          // validate the object.
                          try
                            {
                              object.validate(address, nullptr);
                            }
                          catch (nucleus::Exception const& e)
                            {
                              ELLE_WARN("%s: unable to validate the object",
                                        this);
                              continue;
                            }
                        }
                      */

                      break;
                    }
                  default:
                    {
                      // Validate the block through the common
                      // interface.
                      try
                        {
                          derivable.block().validate(address);
                        }
                      catch (nucleus::Exception const& e)
                        {
                          ELLE_WARN("%s: unable to validate the block",
                                    this);
                          continue;
                        }
                    }
                  case nucleus::neutron::ComponentUnknown:
                    {
                      throw reactor::Exception(elle::concurrency::scheduler(),
                                               elle::sprintf("unknown component '%u'",
                                                             address.component()));
                    }
                  }

                found = true;

                // stop since a block for this specific
                // revision has been retrieved.
                break;
              }

            // check if none if the neighbour has the block.
            if (!found)
              throw reactor::Exception(elle::concurrency::scheduler(),
                                       "unable to retrieve the block associated with "
                                       "the given address from the other peers");
          }

        // Try to retrieve the block from the local storage.
        assert(this->_hole.storage().exist(address, revision));

        // Load the block.
        block = elle::cast<MutableBlock>::runtime(
          this->_hole.storage().load(address, revision));

        // validate the block, depending on its component.
        // although every stored block has been checked, the
        // block may have been corrupt while on the hard disk.
        //
        // note that the Object component requires as additional
        // block for being validated.
        switch (address.component())
          {
          case nucleus::neutron::ComponentObject:
            {
              /* XXX[need to change the way validation works by relying
                     on a callback]
              assert(dynamic_cast<const Object*>(block.get()) != nullptr);
              const Object* object = static_cast<const Object*>(block.get());

              // validate the object according to the presence of
              // a referenced access block.
              if (object->access() !=
                  nucleus::proton::Address::null())
                {
                  // Load the access block.
                  Ptr<nucleus::proton::Block> block
                    (this->_hole.pull(object->access(), nucleus::proton::Revision::Last));
                  Ptr<nucleus::neutron::Access> access
                    (dynamic_cast<nucleus::neutron::Access*>(block.release()));
                  if (access == nullptr)
                    throw reactor::Exception(elle::concurrency::scheduler(),
                                             "expected an access block");

                  // Validate the object.
                  object->validate(address, access.get());
                }
              else
                {
                  // validate the object.
                  object->validate(address, nullptr);
                }
              */

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

        return std::move(block);
      }

      std::unique_ptr<nucleus::proton::Block>
      Machine::get(const nucleus::proton::Address&    address,
                   const nucleus::proton::Revision&    revision)
      {
        ELLE_DEBUG_METHOD(address, revision);

        // Check the machine is connected and has been authenticated
        // as a valid node of the network.
        if (this->_state != State::attached)
          throw elle::Exception{
              "the machine's state '%u' does not allow one "
              "to request operations on the storage layer",
              this->_state};

        if (revision == nucleus::proton::Revision::Last)
          return _get_latest(address);
        else
          return _get_specific(address, revision);
      }

      void
      Machine::wipe(const nucleus::proton::Address&   address)
      {
        ELLE_TRACE_METHOD(address);

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
                switch (address.family())
                  {
                  case nucleus::proton::Family::content_hash_block:
                    {
                      // erase the immutable block.
                      this->_hole.storage().erase(address);

                      break;
                    }
                  case nucleus::proton::Family::public_key_block:
                  case nucleus::proton::Family::owner_key_block:
                  case nucleus::proton::Family::imprint_block:
                    {
                      this->_hole.storage().erase(address);

                      break;
                    }
                  default:
                    {
                      throw elle::Exception("unknown block family %s",
                                            address.family());
                    }
                  }
              }

#ifdef CACHE
              elle::String unique = address.unique();
              auto iterator = cache.find(unique);
              {
                // If the address is present in the cache, remove it.
                if (iterator != cache.end())
                  cache.erase(iterator);
              }
#endif

              // Notify the other hosts of the removal.
              {
                // for every scoutor.
                for (auto neighbour: this->_hosts)
                  {
                    Host* host = neighbour.second;

                    try
                      {
                        host->wipe(address);
                      }
                    catch (std::exception const& e)
                      {
                        ELLE_WARN("%s: remote exception: %s",
                                  this, e.what());
                        continue;
                      }
                  }
              }

              break;
            }
          default:
            {
              throw elle::Exception{
                  "the machine's state '%s' does not allow one to "
                  "request operations on the storage layer",
                  this->_state
              };
            }
          }
      }

      /*----------.
      | Printable |
      `----------*/

      void
      Machine::print(std::ostream& stream) const
      {
        stream << elle::sprintf("Slug machine %s", this->_port);
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
