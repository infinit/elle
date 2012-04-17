//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [wed aug 31 15:04:40 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/format/ini.hxx>

#include <hole/implementations/slug/Machine.hh>
#include <hole/implementations/slug/Manifest.hh>
#include <hole/Hole.hh>

#include <Infinit.hh>

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
      const elle::Natural32             Machine::Timeout = 3000;

//
// ---------- constructors & destructors --------------------------------------
//

      ///
      /// XXX
      ///
      Machine::Machine():
        state(StateDetached),
        port(0),
        timer(NULL),
        synchroniser(NULL)
      {
      }

      ///
      /// XXX
      ///
      Machine::~Machine()
      {
        // delete the timer, if present.
        if (this->timer != NULL)
          delete this->timer;

        // delete the synchroniser, if present.
        if (this->synchroniser != NULL)
          delete this->synchroniser;
      }

//
// ---------- methods ---------------------------------------------------------
//

      ///
      /// XXX
      ///
      elle::Status      Machine::Launch()
      {
        elle::network::Locus     locus;

        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::slug::Machine::Launch()\n");

        //
        // register the messages.
        //
        {
          // register the message.
          if (elle::Network::Register(
                elle::Procedure<TagAuthenticate>(
                  elle::Callback<>::Infer(
                    &Machine::Authenticate, this))) == elle::Status::Error)
            escape("unable to register the callback");

          // register the message.
          if (elle::Network::Register(
                elle::Procedure<TagAuthenticated>(
                  elle::Callback<>::Infer(
                    &Machine::Authenticated, this))) == elle::Status::Error)
            escape("unable to register the callback");

          // register the message.
          if (elle::Network::Register(
                elle::Procedure<TagPush>(
                  elle::Callback<>::Infer(
                    &Machine::Push, this))) == elle::Status::Error)
            escape("unable to register the callback");

          // register the message.
          if (elle::Network::Register(
                elle::Procedure<TagPull>(
                  elle::Callback<>::Infer(
                    &Machine::Pull, this))) == elle::Status::Error)
            escape("unable to register the callback");

          // register the message.
          if (elle::Network::Register(
                elle::Procedure<TagWipe>(
                  elle::Callback<>::Infer(
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
          this->port = Infinit::Configuration["hole"].Get("slug.port", Machine::Default::Port);

          // for every locus in the set.
          for (; iterator != end; ++iterator)
            {
              // allocate the host.
              auto              host = std::unique_ptr<Host>(new Host);

              // create the host.
              if (host->Create(*iterator) == elle::Status::Error)
                escape("unable to create the host");

              // subscribe to the signal.
              if (host->signal.dead.Subscribe(
                    elle::Callback<>::Infer(&Machine::Sweep,
                                            this)) == elle::Status::Error)
                escape("unable to subscribe to the signal");

              // connect the host.
              if (host->Connect() == elle::Status::Error)
                escape("unable to connect the host");

              // add the host to the guestlist.
              if (this->guestlist.Add(host->socket, host.get()) ==
                  elle::Status::Error)
                escape("unable to add the host to the guestlist");

              // release the host.
              host.release();
            }
        }

        //
        // set up the timeout after which the machine will be considered
        // alone in the network, in other words, the very first node.
        //
        {
          // allocate a timer.
          this->timer = new elle::concurrency::Timer;

          // create the timer.
          if (this->timer->Create(
                elle::concurrency::Timer::ModeSingle) == elle::Status::Error)
            escape("unable to create the timer");

          // subscribe to the timer's signal.
          if (this->timer->signal.timeout.Subscribe(
                elle::Callback<>::Infer(&Machine::Alone,
                                        this)) == elle::Status::Error)
            escape("unable to subscribe to the signal");

          // start the timer.
          if (this->timer->Start(Machine::Timeout) == elle::Status::Error)
            escape("unable to start the timer");
        }

        //
        // finally, listen for incoming connections.
        //
        {
          elle::network::Locus   locus;
          elle::Host    host;

          // create a host.
          if (host.Create(elle::Host::TypeAny) == elle::Status::Error)
            escape("unable to create the host");

          // create the listening locus.
          if (locus.Create(host, this->port) == elle::Status::Error)
            escape("unable to create the locus");

          // listen for incoming connections.
          if (elle::TCPServer::Listen(
                locus,
                elle::Callback<>::Infer(
                  &Machine::Connection, this)) == elle::Status::Error)
            escape("unable to listen for TCP connections");
        }

        return elle::Status::Ok;
      }

      ///
      /// this method stores an immutable block.
      ///
      elle::Status      Machine::Put(const nucleus::Address&    address,
                                     const nucleus::ImmutableBlock& block)
      {
        nucleus::Derivable<nucleus::Block>      derivable(address.component,
                                                          block);

        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::slug::Machine::Put[Immutable]()\n");

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

                // for every scoutor.
                for (scoutor = this->neighbourhood.container.begin();
                     scoutor != this->neighbourhood.container.end();
                     scoutor++)
                  {
                    Host*               host = scoutor->second;

                    // send the block to the host.
                    host->socket->Send(
                      elle::Inputs<TagPush>(address, derivable));
                    // XXX do not check the success!

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
        nucleus::Derivable<nucleus::Block>      derivable(address.component,
                                                          block);

        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::slug::Machine::Put[Mutable]()\n");

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

                // for every scoutor.
                for (scoutor = this->neighbourhood.container.begin();
                     scoutor != this->neighbourhood.container.end();
                     scoutor++)
                  {
                    Host*               host = scoutor->second;

                    // send the block to the host.
                    host->socket->Send(
                      elle::Inputs<TagPush>(address, derivable));
                    // XXX do not check the success!

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
        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::slug::Machine::Get[Immutable]()\n");

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
                      Host*                     host = scoutor->second;
                      nucleus::ImmutableBlock*  b;

                      // clone the block by relying on the factory.
                      if (nucleus::Nucleus::Factory.Build(
                            block.component,
                            b) == elle::Status::Error)
                        escape("unable to clone the block");

                      auto                                      ptr =
                        std::unique_ptr<nucleus::ImmutableBlock>(b);
                      nucleus::Derivable<nucleus::Block>        derivable(*b);

                      // request the host.
                      if (host->socket->Call(
                            elle::Inputs<TagPull>(address,
                                                  nucleus::Version::Any),
                            elle::Outputs<TagBlock>(derivable)) ==
                          elle::Status::Ok)
                        {
                          // validate the block.
                          if (b->Validate(address) == elle::Status::Ok)
                            {
                              // finally, store it locally.
                              if (b->Store(Hole::Implementation->network,
                                           address) == elle::Status::Ok)
                                break;
                            }
                        }

                      // ignore the error messages and continue with the
                      // next neighbour.
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
        nucleus::Derivable<nucleus::Block>      derivable(block);

        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::slug::Machine::Get[Mutable]()\n");

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
                      nucleus::MutableBlock*    b;

                      // duplicate the block in its original form.
                      if (nucleus::Nucleus::Factory.Build(
                            block.component,
                            b) == elle::Status::Error)
                        escape("unable to clone the block");

                      auto                                      ptr =
                        std::unique_ptr<nucleus::MutableBlock>(b);
                      nucleus::Derivable<nucleus::Block>        derivable(*b);

                      // request the host.
                      if (host->socket->Call(
                            elle::Inputs<TagPull>(address,
                                                  version),
                            elle::Outputs<TagBlock>(derivable)) ==
                          elle::Status::Ok)
                        {
                          // validate the block, depending on its component.
                          //
                          // indeed, the Object component requires as
                          // additional block for being validated.
                          switch (address.component)
                            {
                            case nucleus::ComponentObject:
                              {
                                const nucleus::Object*  object =
                                  static_cast<const nucleus::Object*>(b);

                                // validate the object according to the
                                // presence of a referenced access block.
                                if (object->meta.access !=
                                    nucleus::Address::Null)
                                  {
                                    nucleus::Access     access;

                                    // load the access block.
                                    if (Hole::Pull(
                                          object->meta.access,
                                          nucleus::Version::Last,
                                          access) == elle::Status::Error)
                                      escape("unable to load the access "
                                             "block");

                                    // validate the object, providing the
                                    if (object->Validate(
                                          address,
                                          access) == elle::Status::Error)
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
                                // validate the block through the common
                                // interface.
                                if (b->Validate(address) == elle::Status::Error)
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
                          b->Store(Hole::Implementation->network,
                                   address);
                          // XXX do not check the result as the block to
                          // XXX store may not be the latest.
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
                          nucleus::MutableBlock*    b;

                          // duplicate the block in its original form.
                          if (nucleus::Nucleus::Factory.Build(
                                block.component,
                                b) == elle::Status::Error)
                            escape("unable to clone the block");

                          auto                                  ptr =
                            std::unique_ptr<nucleus::MutableBlock>(b);
                          nucleus::Derivable<nucleus::Block>    derivable(*b);

                          // request the host.
                          if (host->socket->Call(
                                elle::Inputs<TagPull>(address,
                                                      version),
                                elle::Outputs<TagBlock>(derivable)) ==
                              elle::Status::Ok)
                            {
                              // validate the block, depending on its
                              // component.
                              //
                              // indeed, the Object component requires as
                              // additional block for being validated.
                              switch (address.component)
                                {
                                case nucleus::ComponentObject:
                                  {
                                    const nucleus::Object*  object =
                                      static_cast<const nucleus::Object*>(b);

                                    // validate the object according to the
                                    // presence of a referenced access block.
                                    if (object->meta.access !=
                                        nucleus::Address::Null)
                                      {
                                        nucleus::Access     access;

                                        // load the access block.
                                        if (Hole::Pull(
                                              object->meta.access,
                                              nucleus::Version::Last,
                                              access) == elle::Status::Error)
                                          escape("unable to load the access "
                                                 "block");

                                        // validate the object, providing the
                                        if (object->Validate(
                                              address,
                                              access) == elle::Status::Error)
                                          escape("unable to validate the "
                                                 "object");
                                      }
                                    else
                                      {
                                        // validate the object.
                                        if (object->Validate(
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
                                    if (b->Validate(address) ==
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
                              b->Store(Hole::Implementation->network,
                                       address);
                              // XXX do not check the result as the block to
                              // XXX store may not be the latest i.e when
                              // XXX history is not active.

                              // stop since a block for this specific
                              // version has been retrieved.
                              break;
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
        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::slug::Machine::Kill()\n");

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
                      nucleus::ImmutableBlock   ib;

                      // erase the immutable block.
                      if (ib.Erase(Hole::Implementation->network,
                                   address) == elle::Status::Error)
                        escape("unable to erase the block");

                      break;
                    }
                  case nucleus::FamilyPublicKeyBlock:
                  case nucleus::FamilyOwnerKeyBlock:
                  case nucleus::FamilyImprintBlock:
                    {
                      nucleus::MutableBlock     mb;

                      // retrieve the mutable block.
                      if (mb.Erase(Hole::Implementation->network,
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
                    host->socket->Send(
                      elle::Inputs<TagWipe>(address));

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
        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::slug::Machine::Alone()\n");

        // first, delete the timer.
        bury(timer);

        // reset the pointer.
        this->timer = NULL;

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
      elle::Status      Machine::Connection(elle::network::TCPSocket*    socket)
      {
        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::slug::Machine::Connection()\n");

        // depending on the machine's state.
        switch (this->state)
          {
          case Machine::StateAttached:
            {
              // allocate the host.
              auto      host = std::unique_ptr<Host>(new Host);

              // create the host.
              if (host->Create(socket) == elle::Status::Error)
                escape("unable to create the host");

              // subscribe to the signal.
              if (host->signal.dead.Subscribe(
                    elle::Callback<>::Infer(&Machine::Sweep,
                                            this)) == elle::Status::Error)
                escape("unable to subscribe to the signal");

              // add the host to the guestlist for now until it
              // gets authenticated.
              if (this->guestlist.Add(host->socket,
                                      host.get()) == elle::Status::Error)
                escape("unable to add the host to the neigbourhood");

              // release the host.
              auto      h = host.release();

              // also authenticate to this host now that it is considered a
              // potentiel peer.
              if (h->socket->Send(
                    elle::Inputs<TagAuthenticate>(
                      Hole::Passport,
                      this->port)) == elle::Status::Error)
                escape("unable to send a message");

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

        return elle::Status::Ok;
      }

      ///
      /// XXX
      ///
      elle::Status      Machine::Authenticate(const lune::Passport& passport,
                                              const elle::Port& port)
      {
        Host*           host;
        elle::Session*  session;

        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::slug::Machine::Authenticate()\n");

        // retrieve the network session.
        if (elle::Session::Instance(session) == elle::Status::Error)
          escape("unable to retrieve the current session");

        // if the host exists in the guestlist, handle its authentication.
        if (this->guestlist.Exist(
              static_cast<elle::network::TCPSocket*>(session->socket)) ==
            elle::Status::True)
          {
            Cluster     cluster;

            // retrieve the host from the guestlist.
            if (this->guestlist.Retrieve(
                  static_cast<elle::network::TCPSocket*>(session->socket),
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
            if (host->locus.Create(session->locus.host,
                                   port) == elle::Status::Error)
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
                  elle::Inputs<TagAuthenticated>(
                    cluster)) == elle::Status::Error)
              escape("unable to reply to the caller");

            // also authenticate to this host.
            if (host->socket->Send(
                  elle::Inputs<TagAuthenticate>(
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
        elle::Session*  session;

        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::slug::Machine::Authenticated()\n");

        // retrieve the network session.
        if (elle::Session::Instance(session) == elle::Status::Error)
          escape("unable to retrieve the current session");

        // set the machine as being authenticated and is therefore now
        // considered attached to the network.
        this->state = Machine::StateAttached;

        // set the hole as ready to receive requests.
        if (Hole::Ready() == elle::Status::Error)
          escape("unable to set the hole online");

        // stop the timer, if present.
        if (this->timer != nullptr)
          {
            // first, delete the timer.
            delete this->timer;

            // reset the pointer.
            this->timer = nullptr;
          }

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

              // allocate the host.
              auto      host = std::unique_ptr<Host>(new Host);

              // create the host.
              if (host->Create(locus) == elle::Status::Error)
                escape("unable to create the host");

              // subscribe to the signal.
              if (host->signal.dead.Subscribe(
                    elle::Callback<>::Infer(&Machine::Sweep,
                                            this)) == elle::Status::Error)
                escape("unable to subscribe to the signal");

              // connect the host.
              if (host->Connect() == elle::Status::Error)
                escape("unable to connect the host");

              // add the host to the guestlist.
              if (this->guestlist.Add(host->socket, host.get()) ==
                  elle::Status::Error)
                escape("unable to add the host to the guestlist");

              // release the host.
              host.release();
            }
        }

        //
        // prepare the synchroniser.
        //
        {
          // allocate the synchroniser.
          this->synchroniser = new Synchroniser;

          // subscribe to the signal.
          if (this->synchroniser->signal.synchronised.Subscribe(
                elle::Callback<>::Infer(&Machine::Synchronised,
                                        this)) == elle::Status::Error)
            escape("unable to subscribe to the signal");

          // start the synchroniser.
          if (this->synchroniser->Start() == elle::Status::Error)
            escape("unable to start the synchroniser");
        }

        return elle::Status::Ok;
      }

      ///
      /// XXX
      ///
      elle::Status      Machine::Sweep(Host*                    host)
      {
        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::slug::Machine::Sweep()\n");

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
        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::slug::Machine::Push()\n");

        // bury the synchroniser.
        bury(this->synchroniser);

        // reset its pointer.
        this->synchroniser = NULL;

        return elle::Status::Ok;
      }

      ///
      /// this method stores the given block.
      ///
      elle::Status      Machine::Push(const nucleus::Address&   address,
                                      const
                                        nucleus::Derivable
                                          <nucleus::Block>&     derivable)
      {
        Host*           host;
        elle::Session*  session;
        nucleus::Block* object;

        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::slug::Machine::Push()\n");

        // retrieve the network session.
        if (elle::Session::Instance(session) == elle::Status::Error)
          escape("unable to retrieve the current session");

        // retrieve the host from the guestlist.
        if (this->guestlist.Retrieve(
              static_cast<elle::network::TCPSocket*>(session->socket),
              host) == elle::Status::Error)
          escape("unable to retrieve the host");

        // check the host's state.
        if (host->state != Host::StateAuthenticated)
          escape("unable to process a request from an unauthenticated host");

        // infer the block from the derivable.
        if (derivable.Infer(object) == elle::Status::Error)
          escape("unable to infer the block from the derivable");

        // forward the request depending on the nature of the block which
        // the address indicates.
        switch (address.family)
          {
          case nucleus::FamilyContentHashBlock:
            {
              nucleus::ImmutableBlock*  ib;

              // cast to an immutable block.
              ib = static_cast<nucleus::ImmutableBlock*>(object);

              //
              // first, store the block locally.
              //
              {
                // does the block already exist.
                if (ib->Exist(Hole::Implementation->network,
                              address) == elle::Status::True)
                  escape("this immutable block seems to already exist");

                // store the block.
                if (ib->Store(Hole::Implementation->network,
                              address) == elle::Status::Error)
                  escape("unable to store the block");
              }

              break;
            }
          case nucleus::FamilyPublicKeyBlock:
          case nucleus::FamilyOwnerKeyBlock:
          case nucleus::FamilyImprintBlock:
            {
              nucleus::MutableBlock*    mb;

              // cast to a mutable block.
              mb = static_cast<nucleus::MutableBlock*>(object);

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
                        static_cast<const nucleus::Object*>(mb);

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

                // does the block already exist.
                if (mb->Exist(Hole::Implementation->network,
                              address,
                              nucleus::Version::Last) == elle::Status::True)
                  {
                    nucleus::MutableBlock*      current;

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
                    if (!(mb->version > current->version))
                      escape("the block to store does not seem to derive "
                             "the current version");
                  }

                // store the block.
                if (mb->Store(Hole::Implementation->network,
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
        elle::Session*  session;
        nucleus::Block* block;

        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::slug::Machine::Pull()\n");

        // retrieve the network session.
        if (elle::Session::Instance(session) == elle::Status::Error)
          escape("unable to retrieve the current session");

        // retrieve the host from the guestlist.
        if (this->guestlist.Retrieve(
              static_cast<elle::network::TCPSocket*>(session->socket),
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

        nucleus::Derivable<nucleus::Block>      derivable(address.component,
                                                          *block);

        // return the block.
        if (host->socket->Reply(
              elle::Inputs<TagBlock>(derivable)) == elle::Status::Error)
          escape("unable to return the block");

        return elle::Status::Ok;
      }

      ///
      /// this method removes the block associated with the given address.
      ///
      elle::Status      Machine::Wipe(const nucleus::Address&   address)
      {
        Host*           host;
        elle::Session*  session;

        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::slug::Machine::Wipe()\n");

        // retrieve the network session.
        if (elle::Session::Instance(session) == elle::Status::Error)
          escape("unable to retrieve the current session");

        // retrieve the host from the guestlist.
        if (this->guestlist.Retrieve(
              static_cast<elle::network::TCPSocket*>(session->socket),
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
                nucleus::ImmutableBlock ib;

                // erase the immutable block.
                if (ib.Erase(Hole::Implementation->network,
                             address) == elle::Status::Error)
                  escape("unable to erase the block");

                break;
              }
            case nucleus::FamilyPublicKeyBlock:
            case nucleus::FamilyOwnerKeyBlock:
            case nucleus::FamilyImprintBlock:
              {
                nucleus::MutableBlock   mb;

                // retrieve the mutable block.
                if (mb.Erase(Hole::Implementation->network,
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

        // dump the timer, if present.
        if (this->timer != NULL)
          {
            if (this->timer->Dump(margin + 2) == elle::Status::Error)
              escape("unable to dump the timer");
          }
        else
          {
            std::cout << alignment << elle::Dumpable::Shift
                      << "[Timer] " << elle::none << std::endl;
          }

        // dump the synchroniser, if present.
        if (this->synchroniser != NULL)
          {
            if (this->synchroniser->Dump(margin + 2) == elle::Status::Error)
              escape("unable to dump the synchroniser");
          }
        else
          {
            std::cout << alignment << elle::Dumpable::Shift
                      << "[Synchroniser] " << elle::none << std::endl;
          }

        return elle::Status::Ok;
      }

    }
  }
}
