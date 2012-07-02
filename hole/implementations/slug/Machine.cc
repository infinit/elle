#include <elle/log.hh>
#include <elle/utility/Buffer.hh>
#include <elle/network/Header.hh>
#include <elle/network/Bundle.hh>
#include <elle/standalone/Report.hh>
#include <elle/network/Network.hh>
#include <elle/network/Inputs.hh>
#include <elle/network/Outputs.hh>
#include <elle/standalone/Morgue.hh>
#include <elle/network/TCPSocket.hh>
#include <elle/concurrency/Callback.hh>

#include <reactor/network/exception.hh>
#include <reactor/network/tcp-server.hh>

#include <lune/Passport.hh>

#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Block.hh>
#include <nucleus/proton/Version.hh>

#include <hole/implementations/slug/Cluster.hh>

#include <hole/Hole.hh>
#include <hole/implementations/slug/Machine.hh>
#include <hole/implementations/slug/Manifest.hh>

#include <Infinit.hh>

ELLE_LOG_TRACE_COMPONENT("Infinit.Hole.Slug.Machine");

namespace hole
{
  namespace implementations
  {
    namespace slug
    {

//
// ---------- definitions -----------------------------------------------------
//

      ///
      /// XXX
      ///
      /// XXX corresponds to 's' i.e 19 and 'l' i.e 12 in the alphabet.
      ///
      const elle::Natural16             Machine::Default::Port = 1912;

      ///
      /// XXX 3 seconds
      ///
      const reactor::Duration Machine::Timeout = boost::posix_time::seconds(3);

//
// ---------- constructors & destructors --------------------------------------
//

      ///
      /// XXX
      ///
      Machine::Machine()
        : state(StateDetached)
        , port(0)
        , _server(new reactor::network::TCPServer
                  (elle::concurrency::scheduler()))
      {}

      ///
      /// XXX
      ///
      Machine::~Machine()
      {}

//
// ---------- methods ---------------------------------------------------------
//

      ///
      /// XXX
      ///
      elle::Status      Machine::Launch()
      {
        elle::network::Locus     locus;

        ELLE_LOG_TRACE("Launch");

        //
        // register the messages.
        //
        {
          // register the message.
          if (elle::network::Network::Register(
                elle::network::Procedure<TagAuthenticate>(
                  elle::concurrency::Callback<>::Infer(
                    &Machine::Authenticate, this))) == elle::Status::Error)
            escape("unable to register the callback");

          // register the message.
          if (elle::network::Network::Register(
                elle::network::Procedure<TagAuthenticated>(
                  elle::concurrency::Callback<>::Infer(
                    &Machine::Authenticated, this))) == elle::Status::Error)
            escape("unable to register the callback");

          // register the message.
          if (elle::network::Network::Register(
                elle::network::Procedure<TagPush>(
                  elle::concurrency::Callback<>::Infer(
                    &Machine::Push, this))) == elle::Status::Error)
            escape("unable to register the callback");

          // register the message.
          if (elle::network::Network::Register(
                elle::network::Procedure<TagPull>(
                  elle::concurrency::Callback<>::Infer(
                    &Machine::Pull, this))) == elle::Status::Error)
            escape("unable to register the callback");

          // register the message.
          if (elle::network::Network::Register(
                elle::network::Procedure<TagWipe>(
                  elle::concurrency::Callback<>::Infer(
                    &Machine::Wipe, this))) == elle::Status::Error)
            escape("unable to register the callback");
        }

        //
        // retrieve information from the descriptor.
        //
        {
          std::istringstream    stream;
          auto                  iterator = Hole::Set.loci.begin();
          auto                  end = Hole::Set.loci.end();

          // retrieve the machine's listening port.
          this->port = Infinit::Configuration["hole"].Get("slug.port", 0);

          // for every locus in the set.
          for (; iterator != end; ++iterator)
            {
              std::unique_ptr<Host> host;

              try
                {
                  host = std::unique_ptr<Host>(new Host(*iterator));
                }
              catch (reactor::network::Exception& err)
                {
                  // The host wasn't up, just ignore it.
                  elle::log::warn("Ignore one host:", err.what());
                  continue;
                }

              // subscribe to the signal.
              if (host->signal.dead.Subscribe(
                    elle::concurrency::Callback<>::Infer(&Machine::Sweep,
                                            this)) == elle::Status::Error)
                escape("unable to subscribe to the signal");

              // add the host to the guestlist.
              if (this->guestlist.Add(host->socket, host.get()) ==
                  elle::Status::Error)
                escape("unable to add the host to the guestlist");

              // release the host.
              host.release();
            }
        }

        // Set up the timeout after which the machine will be
        // considered alone in the network, in other words, the very
        // first node.
        elle::concurrency::scheduler().CallLater
          (boost::bind(&Machine::Alone, this),
           "Machine::Alone", Machine::Timeout);

        //
        // finally, listen for incoming connections.
        //
        {
          elle::network::Locus   locus;
          elle::network::Host    host;

          // create a host.
          if (host.Create(elle::network::Host::TypeAny) == elle::Status::Error)
            escape("unable to create the host");

          // create the listening locus.
          if (locus.Create(host, this->port) == elle::Status::Error)
            escape("unable to create the locus");

          _server->listen(locus.port);
          new reactor::Thread(elle::concurrency::scheduler(),
                              "Slug accept",
                              boost::bind(&Machine::_accept, this),
                              true);
        }

        return elle::Status::Ok;
      }

      ///
      /// this method stores an immutable block.
      ///
      elle::Status      Machine::Put(const nucleus::Address&    address,
                                     const nucleus::ImmutableBlock& block)
      {
        ELLE_LOG_TRACE("Put[Immutable]");

        // depending on the machine's state.
        switch (this->state)
          {
          case Machine::StateAttached:
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
                // does the block already exist.
                if (block.Exist(Hole::Implementation->network,
                                address) == elle::Status::True)
                  escape("this immutable block seems to already exist");

                // validate the block.
                if (block.Validate(address) == elle::Status::Error)
                  escape("unable to validate the block");

                // store the block.
                if (block.Store(Hole::Implementation->network,
                                address) == elle::Status::Error)
                  escape("unable to store the block");
              }

              //
              // then, publish it onto the network.
              //
              {
                Neighbourhood::Scoutor  scoutor;

                nucleus::Derivable derivable(address.component, block);

                // for every scoutor.
                for (scoutor = this->neighbourhood.container.begin();
                     scoutor != this->neighbourhood.container.end();
                     scoutor++)
                  {
                    Host*               host = scoutor->second;

                    // send the block to the host.
                    if (host->socket->Send(
                          elle::network::Inputs<TagPush>(address, derivable)) ==
                        elle::Status::Error)
                      {
                        // XXX
                        ELLE_LOG_TRACE("unable to to push the immutable block");
                        address.Dump();
                        block.Dump();
                        host->locus.Dump();
                        show();
                        assert(false);
                      }

                    // ignore the error messages and continue with the
                    // next neighbour.
                    purge();
                  }
              }

              break;
            }
          default:
            {
              escape("the machine's state '%u' does not allow one to request "
                     "operations on the storage layer",
                     this->state);
            }
          }

        return elle::Status::Ok;
      }

      ///
      /// this method stores a mutable block.
      ///
      elle::Status      Machine::Put(const nucleus::Address&    address,
                                     const nucleus::MutableBlock& block)
      {
        ELLE_LOG_TRACE("Put[Mutable]");

        // depending on the machine's state.
        switch (this->state)
          {
          case Machine::StateAttached:
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
                  case nucleus::ComponentObject:
                    {
                      const nucleus::Object*    object =
                        static_cast<const nucleus::Object*>(&block);

                      // validate the object according to the presence of
                      // a referenced access block.
                      if (object->meta.access != nucleus::Address::Null)
                        {
                          nucleus::Access       access;

                          // load the access block.
                          if (Hole::Pull(object->meta.access,
                                         nucleus::Version::Last,
                                         access) == elle::Status::Error)
                            escape("unable to load the access block");

                          // validate the object, providing the
                          if (object->Validate(address,
                                               access) == elle::Status::Error)
                            escape("unable to validate the object");
                        }
                      else
                        {
                          // validate the object.
                          if (object->Validate(
                                address,
                                nucleus::Access::Null) == elle::Status::Error)
                            escape("unable to validate the object");
                        }

                      break;
                    }
                  default:
                    {
                      // validate the block through the common interface.
                      if (block.Validate(address) == elle::Status::Error)
                        escape("the block seems to be invalid");

                      break;
                    }
                  case nucleus::ComponentUnknown:
                    {
                      escape("unknown component '%u'",
                             address.component);
                    }
                  }

                // store the block.
                if (block.Store(Hole::Implementation->network,
                                address) == elle::Status::Error)
                  escape("unable to store the block");
              }

              //
              // then, publish it onto the network.
              //
              {
                Neighbourhood::Scoutor  scoutor;
                nucleus::Derivable derivable(address.component, block);

                // for every scoutor.
                for (scoutor = this->neighbourhood.container.begin();
                     scoutor != this->neighbourhood.container.end();
                     scoutor++)
                  {
                    Host*               host = scoutor->second;

                    // send the block to the host.
                    if (host->socket->Send(
                          elle::network::Inputs<TagPush>(address, derivable)) ==
                        elle::Status::Error)
                      {
                        // XXX
                        ELLE_LOG_TRACE("unable to to push the mutable block");
                        address.Dump();
                        block.Dump();
                        host->locus.Dump();
                        show();
                        assert(false);
                      }

                    // ignore the error messages and continue with the
                    // next neighbour.
                    purge();
                  }
              }

              break;
            }
          default:
            {
              escape("the machine's state '%u' does not allow one to request "
                     "operations on the storage layer",
                     this->state);
            }
          }

        return elle::Status::Ok;
      }

      ///
      /// this method retrieves an immutable block.
      ///
      elle::Status      Machine::Get(const nucleus::Address&    address,
                                     nucleus::ImmutableBlock&   block)
      {
        ELLE_LOG_TRACE("Get[Immutable]");

        // depending on the machine's state.
        switch (this->state)
          {
          case Machine::StateAttached:
            {
              //
              // in this case, the current machine is connected and has
              // been authenticated as a valid node of the network.
              //
              // therefore, the operation is carried out both locally but
              // also sent to every node in the network.
              //

              // does the block exist.
              if (block.Exist(Hole::Implementation->network,
                              address) == elle::Status::True)
                {
                  // load the block.
                  if (block.Load(Hole::Implementation->network,
                                 address) == elle::Status::Error)
                    escape("unable to load the block");

                  // validate the block.
                  if (block.Validate(address) == elle::Status::Error)
                    escape("the block seems to be invalid");
                }
              else
                {
                  //
                  // otherwise, go through the neighbours and retrieve
                  // the block from them.
                  //
                  Neighbourhood::Scoutor                scoutor;

                  // for every scoutor.
                  for (scoutor = this->neighbourhood.container.begin();
                       scoutor != this->neighbourhood.container.end();
                       scoutor++)
                    {
                      nucleus::Derivable derivable;

                      // request the host.
                      Host*                     host = scoutor->second;
                      if (host->socket->Call(
                            elle::network::Inputs<TagPull>(address,
                                                  nucleus::Version::Any),
                            elle::network::Outputs<TagBlock>(derivable)) ==
                          elle::Status::Ok)
                        {
                          auto const& b =
                            static_cast<nucleus::proton::ImmutableBlock const&>(derivable.block());
                          // validate the block.
                          if (b.Validate(address) == elle::Status::Ok)
                            {
                              // finally, store it locally.
                              if (b.Store(Hole::Implementation->network,
                                           address) == elle::Status::Ok)
                                break;
                            }
                          else
                            {
                              // XXX
                              ELLE_LOG_TRACE("unable to validate the immutable block");
                              address.Dump();
                              b.Dump();
                              host->locus.Dump();
                              show();
                              assert(false);
                            }
                        }
                      else
                        {
                          // XXX
                          ELLE_LOG_TRACE("unable to pull the immutable block");
                          address.Dump();
                          host->locus.Dump();
                          show();
                          assert(false);
                        }

                      // ignore the error messages and continue with the
                      // next neighbour.
                      // XXX no log at all in case of error ?
                      purge();
                    }

                  // check if none if the neighbour has the block.
                  if (scoutor == this->neighbourhood.container.end())
                    escape("unable to retrieve the block associated with "
                           "the given address from the other peers");

                  // finally, load the block since it has been retrieved
                  // and stored locally.
                  if (block.Load(Hole::Implementation->network,
                                 address) == elle::Status::Error)
                    escape("unable to load the block");
                }

              break;
            }
          default:
            {
              escape("the machine's state '%u' does not allow one to request "
                     "operations on the storage layer",
                     this->state);
            }
          }

        return elle::Status::Ok;
      }

      ///
      /// this method retrieves a mutable block.
      ///
      elle::Status      Machine::Get(const nucleus::Address&    address,
                                     const nucleus::Version&    version,
                                     nucleus::MutableBlock&     block)
      {
        //nucleus::Derivable<nucleus::Block>      derivable(block);

        ELLE_LOG_TRACE("Get[Mutable]");

        // depending on the machine's state.
        switch (this->state)
          {
          case Machine::StateAttached:
            {
              //
              // in this case, the current machine is connected and has
              // been authenticated as a valid node of the network.
              //

              //
              // first, do something special: all the hosts are contacted
              // in order to retrieve the latest version of the block, if
              // the latest version is requested.
              //
              // this is required since no synchronisation mechanism is
              // present yet so the current machine may have missed some
              // versions when disconnected.
              //
              if (version == nucleus::Version::Last)
                {
                  Neighbourhood::Scoutor        scoutor;

                  // for every scoutor.
                  for (scoutor = this->neighbourhood.container.begin();
                       scoutor != this->neighbourhood.container.end();
                       scoutor++)
                    {
                      Host*                     host = scoutor->second;

                      nucleus::Derivable        derivable;

                      // request the host.
                      if (host->socket->Call(
                            elle::network::Inputs<TagPull>(address, version),
                            elle::network::Outputs<TagBlock>(derivable)) ==
                          elle::Status::Ok)
                        {
                          auto const& mb =
                            static_cast<nucleus::proton::MutableBlock const&>(
                                derivable.block());

                          // validate the block, depending on its component.
                          //
                          // indeed, the Object component requires as
                          // additional block for being validated.
                          switch (address.component)
                            {
                            case nucleus::ComponentObject:
                              {
                                nucleus::Object const&  object =
                                  static_cast<nucleus::Object const&>(mb);

                                // validate the object according to the
                                // presence of a referenced access block.
                                if (object.meta.access !=
                                    nucleus::Address::Null)
                                  {
                                    nucleus::Access     access;

                                    // load the access block.
                                    if (Hole::Pull(
                                          object.meta.access,
                                          nucleus::Version::Last,
                                          access) == elle::Status::Error)
                                      escape("unable to load the access "
                                             "block");

                                    // validate the object, providing the
                                    if (object.Validate(
                                          address,
                                          access) == elle::Status::Error)
                                      escape("unable to validate the object");
                                  }
                                else
                                  {
                                    // validate the object.
                                    if (object.Validate(
                                          address,
                                          nucleus::Access::Null) ==
                                        elle::Status::Error)
                                      escape("unable to validate the object");
                                  }

                                break;
                              }
                            default:
                              {
                                // validate the block through the common
                                // interface.
                                if (mb.Validate(address) == elle::Status::Error)
                                  escape("the block seems to be invalid");

                                break;
                              }
                            case nucleus::ComponentUnknown:
                              {
                                escape("unknown component '%u'",
                                       address.component);
                              }
                            }

                          // finally, since the block has been retrieved,
                          // store it locally.
                          mb.Store(Hole::Implementation->network, address);
                          // XXX do not check the result as the block to
                          // XXX store may not be the latest.
                        }
                      else
                        {
                          // XXX
                          ELLE_LOG_TRACE("unable to pull the latest mutable block");
                          address.Dump();
                          host->locus.Dump();
                          show();
                          assert(false);
                        }

                      // ignore the error messages and continue with the
                      // next neighbour.
                      purge();
                    }

                  //
                  // at this point, we may have retrieved one or more versions
                  // of the mutable block but we do not have any guarantee.
                  //

                  // does the block exist.
                  if (block.Exist(Hole::Implementation->network,
                                  address,
                                  version) == elle::Status::True)
                    {
                      // load the block.
                      if (block.Load(Hole::Implementation->network,
                                     address,
                                     version) == elle::Status::Error)
                        escape("unable to load the block");

                      // validate the block, depending on its component.
                      // although every stored block has been checked, the
                      // block may have been corrupt while on the hard disk.
                      //
                      // note that the Object component requires as additional
                      // block for being validated.
                      switch (address.component)
                        {
                        case nucleus::ComponentObject:
                          {
                            const nucleus::Object*  object =
                              static_cast<const nucleus::Object*>(&block);

                            // validate the object according to the presence of
                            // a referenced access block.
                            if (object->meta.access != nucleus::Address::Null)
                              {
                                nucleus::Access     access;

                                // load the access block.
                                if (Hole::Pull(object->meta.access,
                                               nucleus::Version::Last,
                                               access) == elle::Status::Error)
                                  escape("unable to load the access block");

                                // validate the object, providing the
                                if (object->Validate(address,
                                                     access) ==
                                    elle::Status::Error)
                                  escape("unable to validate the object");
                              }
                            else
                              {
                                // validate the object.
                                if (object->Validate(
                                      address,
                                      nucleus::Access::Null) ==
                                    elle::Status::Error)
                                  escape("unable to validate the object");
                              }

                            break;
                          }
                        default:
                          {
                            // validate the block through the common interface.
                            if (block.Validate(address) == elle::Status::Error)
                              escape("the block seems to be invalid");

                            break;
                          }
                        case nucleus::ComponentUnknown:
                          {
                            escape("unknown component '%u'",
                                   address.component);
                          }
                        }
                    }
                }
              else
                {
                  //
                  // otherwise, go through the neighbours and retrieve the
                  // specific version of the block from them.
                  //

                  // does the block exist: if it does not, retrieve it
                  // from the peers.
                  if (block.Exist(Hole::Implementation->network,
                                  address,
                                  version) == elle::Status::False)
                    {
                      Neighbourhood::Scoutor        scoutor;

                      // for every scoutor.
                      for (scoutor = this->neighbourhood.container.begin();
                           scoutor != this->neighbourhood.container.end();
                           scoutor++)
                        {
                          Host*                     host = scoutor->second;
                          nucleus::Derivable        derivable;

                          // request the host.
                          if (host->socket->Call(
                                elle::network::Inputs<TagPull>(address, version),
                                elle::network::Outputs<TagBlock>(derivable)) ==
                              elle::Status::Ok)
                            {
                              auto const& mb=
                                static_cast<nucleus::MutableBlock const&>(
                                    derivable.block());
                              // validate the block, depending on its
                              // component.
                              //
                              // indeed, the Object component requires as
                              // additional block for being validated.
                              switch (address.component)
                                {
                                case nucleus::ComponentObject:
                                  {
                                    nucleus::Object const&  object =
                                      static_cast<nucleus::Object const&>(derivable.block());

                                    // validate the object according to the
                                    // presence of a referenced access block.
                                    if (object.meta.access !=
                                        nucleus::Address::Null)
                                      {
                                        nucleus::Access     access;

                                        // load the access block.
                                        if (Hole::Pull(
                                              object.meta.access,
                                              nucleus::Version::Last,
                                              access) == elle::Status::Error)
                                          escape("unable to load the access "
                                                 "block");

                                        // validate the object, providing the
                                        if (object.Validate(
                                              address,
                                              access) == elle::Status::Error)
                                          escape("unable to validate the "
                                                 "object");
                                      }
                                    else
                                      {
                                        // validate the object.
                                        if (object.Validate(
                                              address,
                                              nucleus::Access::Null) ==
                                            elle::Status::Error)
                                          escape("unable to validate the "
                                                 "object");
                                      }

                                    break;
                                  }
                                default:
                                  {
                                    // validate the block through the common
                                    // interface.
                                    if (derivable.block().Validate(address) ==
                                        elle::Status::Error)
                                      escape("the block seems to be invalid");

                                    break;
                                  }
                                case nucleus::ComponentUnknown:
                                  {
                                    escape("unknown component '%u'",
                                           address.component);
                                  }
                                }

                              // finally, since the block has been retrieved,
                              // store it locally.
                              mb.Store(Hole::Implementation->network, address);
                              // XXX do not check the result as the block to
                              // XXX store may not be the latest i.e when
                              // XXX history is not active.

                              // stop since a block for this specific
                              // version has been retrieved.
                              break;
                            }
                          else
                            {
                              // XXX
                              ELLE_LOG_TRACE("unable to pull the versioned mutable block");
                              address.Dump();
                              version.Dump();
                              host->locus.Dump();
                              show();
                              assert(false);
                            }

                          // ignore the error messages and continue with the
                          // next neighbour.
                          purge();
                        }

                      // check if none if the neighbour has the block.
                      if (scoutor == this->neighbourhood.container.end())
                        escape("unable to retrieve the block associated with "
                               "the given address from the other peers");
                    }

                  //
                  // now let us try to retrieve the block from the
                  // local storage.
                  //

                  // does the block exist.
                  if (block.Exist(Hole::Implementation->network,
                                  address,
                                  version) == elle::Status::True)
                    {
                      // load the block.
                      if (block.Load(Hole::Implementation->network,
                                     address,
                                     version) == elle::Status::Error)
                        escape("unable to load the block");

                      // validate the block, depending on its component.
                      // although every stored block has been checked, the
                      // block may have been corrupt while on the hard disk.
                      //
                      // note that the Object component requires as additional
                      // block for being validated.
                      switch (address.component)
                        {
                        case nucleus::ComponentObject:
                          {
                            const nucleus::Object*  object =
                              static_cast<const nucleus::Object*>(&block);

                            // validate the object according to the presence of
                            // a referenced access block.
                            if (object->meta.access != nucleus::Address::Null)
                              {
                                nucleus::Access     access;

                                // load the access block.
                                if (Hole::Pull(object->meta.access,
                                               nucleus::Version::Last,
                                               access) == elle::Status::Error)
                                  escape("unable to load the access block");

                                // validate the object, providing the
                                if (object->Validate(address,
                                                     access) ==
                                    elle::Status::Error)
                                  escape("unable to validate the object");
                              }
                            else
                              {
                                // validate the object.
                                if (object->Validate(
                                      address,
                                      nucleus::Access::Null) ==
                                    elle::Status::Error)
                                  escape("unable to validate the object");
                              }

                            break;
                          }
                        default:
                          {
                            // validate the block through the common interface.
                            if (block.Validate(address) == elle::Status::Error)
                              escape("the block seems to be invalid");

                            break;
                          }
                        case nucleus::ComponentUnknown:
                          {
                            escape("unknown component '%u'",
                                   address.component);
                          }
                        }
                    }
                }

              break;
            }
          default:
            {
              escape("the machine's state '%u' does not allow one "
                     "to request operations on the storage layer",
                     this->state);
            }
          }

        return elle::Status::Ok;
      }

      ///
      /// this method removes a block.
      ///
      elle::Status      Machine::Kill(const nucleus::Address&   address)
      {
        ELLE_LOG_TRACE("Kill");

        // depending on the machine's state.
        switch (this->state)
          {
          case Machine::StateAttached:
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
                  case nucleus::FamilyContentHashBlock:
                    {
                      // erase the immutable block.
                      if (nucleus::ImmutableBlock::Erase(
                            Hole::Implementation->network,
                            address) == elle::Status::Error)
                        escape("unable to erase the block");

                      break;
                    }
                  case nucleus::FamilyPublicKeyBlock:
                  case nucleus::FamilyOwnerKeyBlock:
                  case nucleus::FamilyImprintBlock:
                    {
                      // retrieve the mutable block.
                      if (nucleus::MutableBlock::Erase(
                            Hole::Implementation->network,
                            address) == elle::Status::Error)
                        escape("unable to erase the block");

                      break;
                    }
                  default:
                    {
                      escape("unknown block family");
                    }
                  }
              }

              //
              // then, notify the other hosts of the removal.
              //
              {
                Neighbourhood::Scoutor  scoutor;

                // for every scoutor.
                for (scoutor = this->neighbourhood.container.begin();
                     scoutor != this->neighbourhood.container.end();
                     scoutor++)
                  {
                    Host*               host = scoutor->second;

                    // send the request to the host.
                    // XXX do not check the success!
                    if (host->socket->Send(
                          elle::network::Inputs<TagWipe>(address)) ==
                        elle::Status::Error)
                      {
                        // XXX
                        ELLE_LOG_TRACE("unable to wipe the block");
                        address.Dump();
                        show();
                        assert(false);
                      }

                    // ignore the error messages and continue with the
                    // next neighbour.
                    purge();
                  }
              }

              break;
            }
          default:
            {
              escape("the machine's state '%u' does not allow one to "
                     "request operations on the storage layer",
                     this->state);
            }
          }

        return elle::Status::Ok;
      }

//
// ---------- callbacks -------------------------------------------------------
//

      ///
      /// XXX
      ///
      elle::Status      Machine::Alone()
      {
        ELLE_LOG_TRACE("Alone");

        // if the machine has been neither connected nor authenticated
        // to existing nodes...
        if (this->state == Machine::StateDetached)
          {
            // then, suppose that the current machine as the only one in
            // the network.
            //
            // thus, it can be implicitly considered as authenticated
            // in a network composed of itself alone.
            this->state = Machine::StateAttached;

            // set the hole as ready to receive requests.
            if (Hole::Ready() == elle::Status::Error)
              escape("unable to set the hole online");
          }

        return elle::Status::Ok;
      }

      ///
      /// this method handles new connections.
      ///
      void
      Machine::_accept()
      {
        while (true)
          {
            reactor::network::TCPSocket* socket = _server->accept();
            auto connection = new elle::network::TCPSocket(socket);

            ELLE_LOG_TRACE("_accept");

            // depending on the machine's state.
            switch (this->state)
              {
                case Machine::StateAttached:
                {
                  // allocate the host.
                  auto      host = std::unique_ptr<Host>(new Host(connection));

                  // subscribe to the signal.
                  if (host->signal.dead.Subscribe(
                        elle::concurrency::Callback<>::Infer(
                          &Machine::Sweep,
                          this)) == elle::Status::Error)
                    throw std::runtime_error("unable to subscribe to the signal");

                  // add the host to the guestlist for now until it
                  // gets authenticated.
                  if (this->guestlist.Add(host->socket,
                                          host.get()) == elle::Status::Error)
                    throw std::runtime_error("unable to add the host to the neigbourhood");

                  // release the host.
                  auto      h = host.release();

                  // also authenticate to this host now that it is
                  // considered a potentiel peer.
                  if (h->socket->Send(
                        elle::network::Inputs<TagAuthenticate>(
                          Hole::Passport,
                          this->port)) == elle::Status::Error)
                    throw std::runtime_error("unable to send a message");

                  break;
                }
                default:
                {
                  //
                  // if the machine is not authenticated, reject the
                  // incoming connection right away.
                  //

                  // delete the socket. XXX ARGHLLL use unique_ptr !
                  delete socket;

                  break;
                }
              }
          }
      }

      ///
      /// XXX
      ///
      elle::Status      Machine::Authenticate(const lune::Passport& passport,
                                              const elle::network::Port& port)
      {
        Host*           host;

        ELLE_LOG_TRACE("Authenticate");

        // if the host exists in the guestlist, handle its authentication.
        if (this->guestlist.Exist(
              static_cast<elle::network::TCPSocket*>(elle::network::current_context().socket)) ==
            elle::Status::True)
          {
            Cluster     cluster;

            // retrieve the host from the guestlist.
            if (this->guestlist.Retrieve(
                  static_cast<elle::network::TCPSocket*>(elle::network::current_context().socket),
                  host) == elle::Status::Error)
              escape("unable to retrieve the host");

            // if the host has already been authenticated, do nothing.
            if (host->state == Host::StateAuthenticated)
              return elle::Status::Ok;

            // create the cluster based on the current neighbourhood.
            if (cluster.Create(this->neighbourhood) == elle::Status::Error)
              escape("unable to create the cluster");

            // validate the passport.
            if (passport.Validate(Infinit::Authority) == elle::Status::Error)
              escape("unable to validate the passport");

            // create the host's locus according to the given port.
            elle::network::Host h;
            h.Create(elle::network::current_context().host);
            if (host->locus.Create(h, port) == elle::Status::Error)
              escape("unable to create the locus");

            // add the host to the neighbourhood now that it has been
            // authenticated.
            if (this->neighbourhood.Add(host->locus,
                                        host) == elle::Status::Error)
              escape("unable to add the host to the neighbourhood");

            // set the host as having been authenticated.
            if (host->Authenticated() == elle::Status::Error)
              escape("unable to set the host as authenticated");

            // reply.
            if (host->socket->Reply(
                  elle::network::Inputs<TagAuthenticated>(
                    cluster)) == elle::Status::Error)
              escape("unable to reply to the caller");

            // also authenticate to this host.
            if (host->socket->Send(
                  elle::network::Inputs<TagAuthenticate>(
                    Hole::Passport,
                    this->port)) == elle::Status::Error)
              escape("unable to send a message");
          }
        else
          {
            log("unexpected: an Authenticate message has been received "
                "from an unknown guest");
          }

        return elle::Status::Ok;
      }

      ///
      /// XXX
      ///
      elle::Status      Machine::Authenticated(const Cluster&   cluster)
      {
        ELLE_LOG_TRACE("Authenticated");

        // set the machine as being authenticated and is therefore now
        // considered attached to the network.
        this->state = Machine::StateAttached;

        // set the hole as ready to receive requests.
        if (Hole::Ready() == elle::Status::Error)
          escape("unable to set the hole online");

        //
        // use the given cluster to extend the network by connecting to
        // every other host.
        //
        {
          Cluster::Scoutor      scoutor;

          // go through the cluster.
          for (scoutor = cluster.container.begin();
               scoutor != cluster.container.end();
               scoutor++)
            {
              elle::network::Locus       locus = *scoutor;

              // check if this locus is already registered.
              if (this->neighbourhood.Exist(locus) == elle::Status::True)
                continue;

              auto host = std::unique_ptr<Host>(new Host(locus));

              // subscribe to the signal.
              if (host->signal.dead.Subscribe(
                    elle::concurrency::Callback<>::Infer(
                      &Machine::Sweep,
                      this)) == elle::Status::Error)
                escape("unable to subscribe to the signal");

              // add the host to the guestlist.
              if (this->guestlist.Add(host->socket, host.get()) ==
                  elle::Status::Error)
                escape("unable to add the host to the guestlist");

              // release the host.
              host.release();
            }
        }

        return elle::Status::Ok;
      }

      ///
      /// XXX
      ///
      elle::Status      Machine::Sweep(Host*                    host)
      {
        ELLE_LOG_TRACE("Sweep");

        // depending on the host's state.
        switch (host->state)
          {
          case Host::StateAuthenticated:
            {
              // remove the host from the guestlist.
              if (this->guestlist.Remove(host->socket) == elle::Status::Error)
                escape("unable to remove the host from the guestlist");

              // remove the host from the neighbourhood.
              if (this->neighbourhood.Remove(host->locus) == elle::Status::Error)
                escape("unable to remove the host from the neighbourhood");

              break;
            }
          default:
            {
              // does the host exist in the guestlist.
              if (this->guestlist.Exist(host->socket) == elle::Status::True)
                {
                  // remove the host from the guestlist.
                  if (this->guestlist.Remove(host->socket) ==
                      elle::Status::Error)
                    escape("unable to remove the host from the neighbourhood");
                }

              break;
            }
          }

        // delete the host.
        bury(host);

        return elle::Status::Ok;
      }

      ///
      /// XXX
      ///
      elle::Status      Machine::Synchronised()
      {
        ELLE_LOG_TRACE("Push");

        return elle::Status::Ok;
      }

      ///
      /// this method stores the given block.
      ///
      elle::Status      Machine::Push(const nucleus::Address&   address,
                                      const nucleus::Derivable& derivable)
      {
        Host*           host;

        ELLE_LOG_TRACE("Push");
        auto const& block =
          static_cast<nucleus::Derivable const&>(derivable).block();

        // retrieve the host from the guestlist.
        if (this->guestlist.Retrieve(
              elle::network::current_context().socket,
              host) == elle::Status::Error)
          escape("unable to retrieve the host");

        // check the host's state.
        if (host->state != Host::StateAuthenticated)
          escape("unable to process a request from an unauthenticated host");

        // forward the request depending on the nature of the block which
        // the address indicates.
        switch (address.family)
          {
          case nucleus::FamilyContentHashBlock:
            {
              nucleus::ImmutableBlock const& ib =
                  static_cast<nucleus::ImmutableBlock const&>(block);
              assert(dynamic_cast<nucleus::ImmutableBlock const*>(&block));
              ELLE_LOG_TRACE("pushing immutable block");

              //
              // first, store the block locally.
              //
              {
                // does the block already exist.
                if (ib.Exist(Hole::Implementation->network,
                              address) == elle::Status::True)
                  escape("this immutable block seems to already exist");

                // store the block.
                if (ib.Store(Hole::Implementation->network,
                              address) == elle::Status::Error)
                  escape("unable to store the block");
              }

              break;
            }
          case nucleus::FamilyPublicKeyBlock:
          case nucleus::FamilyOwnerKeyBlock:
          case nucleus::FamilyImprintBlock:
            {
              nucleus::MutableBlock const& mb =
                  static_cast<nucleus::MutableBlock const&>(block);
              assert(dynamic_cast<nucleus::MutableBlock const*>(&block));

              ELLE_LOG_TRACE("pushing mutable block");

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
                  case nucleus::ComponentObject:
                    {
                      const nucleus::Object*    object =
                        static_cast<const nucleus::Object*>(&mb);

                      ELLE_LOG_TRACE("validating the object mutable block");

                      // validate the object according to the presence of
                      // a referenced access block.
                      if (object->meta.access != nucleus::Address::Null)
                        {
                          nucleus::Access       access;

                          ELLE_LOG_TRACE("retrieving the access block");

                          // load the access block.
                          if (Hole::Pull(object->meta.access,
                                         nucleus::Version::Last,
                                         access) == elle::Status::Error)
                            escape("unable to load the access block");

                          // validate the object, providing the
                          if (object->Validate(address,
                                               access) == elle::Status::Error)
                            escape("unable to validate the object");
                        }
                      else
                        {
                          // validate the object.
                          if (object->Validate(
                                address,
                                nucleus::Access::Null) == elle::Status::Error)
                            escape("unable to validate the object");
                        }

                      break;
                    }
                  default:
                    {
                      ELLE_LOG_TRACE("validating the mutable block");

                      // validate the block through the common interface.
                      if (mb.Validate(address) == elle::Status::Error)
                        escape("the block seems to be invalid");

                      break;
                    }
                  case nucleus::ComponentUnknown:
                    {
                      escape("unknown component '%u'",
                             address.component);
                    }
                  }

                // does the block already exist.
                if (mb.Exist(Hole::Implementation->network,
                              address,
                              nucleus::Version::Last) == elle::Status::True)
                  {
                    nucleus::MutableBlock*      current;

                    ELLE_LOG_TRACE("the mutable block seems to exist "
                                   "locally, make sure it derives the "
                                   "current version");

                    // build a block according to the component.
                    if (nucleus::Nucleus::Factory.Build(address.component,
                                                        current) ==
                        elle::Status::Error)
                      escape("unable to build the block");

                    std::unique_ptr<nucleus::MutableBlock> guard(current);

                    // load the latest version.
                    if (current->Load(Hole::Implementation->network,
                                      address,
                                      nucleus::Version::Last) ==
                        elle::Status::Error)
                      escape("unable to load the current version");

                    // does the given block derive the current version.
                    if (!(mb.version > current->version))
                      escape("the block to store does not seem to derive "
                             "the current version");
                  }

                ELLE_LOG_TRACE("now storing the validated mutable block locally");

                // store the block.
                if (mb.Store(Hole::Implementation->network,
                              address) == elle::Status::Error)
                  escape("unable to store the block");
              }

              break;
            }
          default:
            {
              escape("unknown block family");
            }
          }

        // XXX do not even bother returning TagOk

        return elle::Status::Ok;
      }

      ///
      /// this method returns the block associated with the given address.
      ///
      elle::Status      Machine::Pull(const nucleus::Address&   address,
                                      const nucleus::Version&   version)
      {
        Host*           host;
        nucleus::Block* block;

        ELLE_LOG_TRACE("Pull");

        // retrieve the host from the guestlist.
        if (this->guestlist.Retrieve(
              elle::network::current_context().socket,
              host) == elle::Status::Error)
          escape("unable to retrieve the host");

        // check the host's state.
        if (host->state != Host::StateAuthenticated)
          escape("unable to process a request from an unauthenticated host");

        // build the block according to the component.
        if (nucleus::Nucleus::Factory.Build(address.component,
                                            block) == elle::Status::Error)
          escape("unable to build the block");

        std::unique_ptr<nucleus::Block> guard(block);

        // forward the request depending on the nature of the block which
        // the addres indicates.
        switch (address.family)
          {
          case nucleus::FamilyContentHashBlock:
            {
              nucleus::ImmutableBlock*  ib;

              ELLE_LOG_TRACE("pulling immutable block");

              // cast to an immutable block.
              ib = static_cast<nucleus::ImmutableBlock*>(block);

              // does the block exist.
              if (ib->Exist(Hole::Implementation->network,
                            address) == elle::Status::False)
                escape("the block does not seem to exist");

              // load the block.
              if (ib->Load(Hole::Implementation->network,
                           address) == elle::Status::Error)
                escape("unable to load the block");

              // validate the block.
              if (ib->Validate(address) == elle::Status::Error)
                escape("the block seems to be invalid");

              break;
            }
          case nucleus::FamilyPublicKeyBlock:
          case nucleus::FamilyOwnerKeyBlock:
          case nucleus::FamilyImprintBlock:
            {
              nucleus::MutableBlock*    mb;

              ELLE_LOG_TRACE("pulling mutable block");

              // cast to a mutable block.
              mb = static_cast<nucleus::MutableBlock*>(block);

              // does the block exist.
              if (mb->Exist(Hole::Implementation->network,
                            address, version) == elle::Status::True)
                {
                  // load the block.
                  if (mb->Load(Hole::Implementation->network,
                               address, version) == elle::Status::Error)
                    escape("unable to load the block");

                  // validate the block, depending on its component.
                  //
                  // indeed, the Object component requires as additional
                  // block for being validated.
                  switch (address.component)
                    {
                    case nucleus::ComponentObject:
                      {
                        const nucleus::Object*  object =
                          static_cast<const nucleus::Object*>(mb);

                        // validate the object according to the presence of
                        // a referenced access block.
                        if (object->meta.access != nucleus::Address::Null)
                          {
                            nucleus::Access     access;

                            // load the access block.
                            if (Hole::Pull(object->meta.access,
                                           nucleus::Version::Last,
                                           access) == elle::Status::Error)
                              escape("unable to load the access block");

                            // validate the object, providing the
                            if (object->Validate(address,
                                                 access) == elle::Status::Error)
                              escape("unable to validate the object");
                          }
                        else
                          {
                            // validate the object.
                            if (object->Validate(
                                  address,
                                  nucleus::Access::Null) == elle::Status::Error)
                              escape("unable to validate the object");
                          }

                        break;
                      }
                    default:
                      {
                        // validate the block through the common interface.
                        if (mb->Validate(address) == elle::Status::Error)
                          escape("the block seems to be invalid");

                        break;
                      }
                    case nucleus::ComponentUnknown:
                      {
                        escape("unknown component '%u'",
                               address.component);
                      }
                    }
                }

              break;
            }
          default:
            {
              escape("unknown block family");
            }
          }

        nucleus::Derivable derivable(address.component, *block);

        // return the block.
        if (host->socket->Reply(
              elle::network::Inputs<TagBlock>(derivable)) == elle::Status::Error)
          escape("unable to return the block");

        return elle::Status::Ok;
      }

      ///
      /// this method removes the block associated with the given address.
      ///
      elle::Status      Machine::Wipe(const nucleus::Address&   address)
      {
        Host*           host;

        ELLE_LOG_TRACE("Wipe");

        // retrieve the host from the guestlist.
        if (this->guestlist.Retrieve(
              static_cast<elle::network::TCPSocket*>(elle::network::current_context().socket),
              host) == elle::Status::Error)
          escape("unable to retrieve the host");

        // check the host's state.
        if (host->state != Host::StateAuthenticated)
          escape("unable to process a request from an unauthenticated host");

        //
        // remove the block locally.
        //
        {
          // treat the request depending on the nature of the block which
          // the addres indicates.
          switch (address.family)
            {
            case nucleus::FamilyContentHashBlock:
              {
                // erase the immutable block.
                if (nucleus::ImmutableBlock::Erase(Hole::Implementation->network,
                             address) == elle::Status::Error)
                  escape("unable to erase the block");

                break;
              }
            case nucleus::FamilyPublicKeyBlock:
            case nucleus::FamilyOwnerKeyBlock:
            case nucleus::FamilyImprintBlock:
              {
                // retrieve the mutable block.
                if (nucleus::MutableBlock::Erase(Hole::Implementation->network,
                             address) == elle::Status::Error)
                  escape("unable to erase the block");

                break;
              }
            default:
              {
                escape("unknown block family");
              }
            }
        }

        // XXX do not even bother returning TagOk

        return elle::Status::Ok;
      }

//
// ---------- dumpable --------------------------------------------------------
//

      ///
      /// this method dumps the machine.
      ///
      elle::Status      Machine::Dump(const elle::Natural32     margin) const
      {
        elle::String    alignment(margin, ' ');

        std::cout << alignment << "[Machine]" << std::endl;

        // dump the state.
        std::cout << alignment << elle::Dumpable::Shift
                  << "[State] " << this->state << std::endl;

        // dump the port.
        std::cout << alignment << elle::Dumpable::Shift
                  << "[Port] " << std::dec << this->port << std::endl;

        // dump the guestlist.
        if (this->guestlist.Dump(margin + 2) == elle::Status::Error)
          escape("unable to dump the guestlist");

        // dump the neighbourhood.
        if (this->neighbourhood.Dump(margin + 2) == elle::Status::Error)
          escape("unable to dump the neighbourhood");

        return elle::Status::Ok;
      }
    }
  }
}
