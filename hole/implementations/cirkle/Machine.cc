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

#include <hole/implementations/cirkle/Machine.hh>
#include <hole/implementations/cirkle/Manifest.hh>
#include <hole/Hole.hh>

#include <Infinit.hh>

namespace hole
{
  namespace implementations
  {
    namespace cirkle
    {

//
// ---------- definitions -----------------------------------------------------
//

      ///
      /// XXX
      ///
      /// XXX corresponds to 'c' i.e 3, 'i' i.e 9 and 'r' i.e 18 in the
      /// alphabet
      ///
      const elle::Natural16             Machine::Default::Port = 3918;

      ///
      /// XXX 5 minutes: 300000
      ///
      const elle::Natural32             Machine::Frequency = 10000;

//
// ---------- constructors & destructors --------------------------------------
//

      ///
      /// XXX
      ///
      Machine::Machine():
        state(Machine::StateUnauthenticated)
      {
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


        //
        // register the messages.
        //
        {
          // register the message.
          if (elle::Network::Register(
                elle::Procedure<TagChallenge>(
                  elle::Callback<>::Infer(
                    &Machine::Challenge, this))) == elle::Status::Error)
            escape("unable to register the callback");

          // register the message.
          if (elle::Network::Register(
                elle::Procedure<TagPassport>(
                  elle::Callback<>::Infer(
                    &Machine::Passport, this))) == elle::Status::Error)
            escape("unable to register the callback");

          // register the message.
          if (elle::Network::Register(
                elle::Procedure<TagPort>(
                  elle::Callback<>::Infer(
                    &Machine::Port, this))) == elle::Status::Error)
            escape("unable to register the callback");

          // register the message.
          if (elle::Network::Register(
                elle::Procedure<TagAuthenticated>(
                  elle::Callback<>::Infer(
                    &Machine::Authenticated, this))) == elle::Status::Error)
            escape("unable to register the callback");

          // register the message.
          if (elle::Network::Register(
                elle::Procedure<TagUpdate>(
                  elle::Callback<>::Infer(
                    &Machine::Update, this))) == elle::Status::Error)
            escape("unable to register the callback");
        }

        //
        // retrieve information from the descriptor.
        //
        {
          elle::String  string;

          // XXX improve this with getting a list of hosts.

          // retrieve the neighbours' loci.
          if (Hole::Descriptor.Get("cirkle", "neighbours",
                                   string) == elle::Status::Error)
            escape("unable to retrieve the cirkle's host address from the "
                   "network descriptor");

          // build the host locus.
          if (locus.Create(string) == elle::Status::Error)
            escape("unable to create the host locus");

          // retrieve the machine's listening port.
          if (Hole::Descriptor.Get("cirkle", "port",
                                   this->port,
                                   Machine::Default::Port) ==
              elle::Status::Error)
            escape("unable to retrieve the cirkle's local port from the "
                   "network descriptor");
        }

        //
        // create the neighbour.
        //
        {
          Neighbour*    neighbour;

          // XXX improve this with getting a list of hosts.

          enterx(instance(neighbour));

          // allocate the neighbour.
          neighbour = new Neighbour;

          // create the neighbour.
          if (neighbour->Create(locus) == elle::Status::Error)
            escape("unable to create the neighbour");

          // add the neighbour to the neighbourhood.
          if (this->neighbourhood.Add(neighbour->locus,
                                      neighbour) == elle::Status::Error)
            escape("unable to add the neighbour to the neighbourhood");

          // connect the neighbour.
          if (neighbour->Connect() == elle::Status::Error)
            escape("unable to connect the neighbour");

          // sleep for some time.
          // XXX check if connected -> if not, automatically authed

          // waive.
          waive(neighbour);
        }

        //
        // listen for incoming connections
        //
        {
          elle::network::Locus                   locus;
          elle::Host                    host;

          // create a host.
          if (host.Create(elle::Host::TypeAny) == elle::Status::Error)
            escape("unable to create the host");

          // create the listening locus.
          if (locus.Create(host, this->port) == elle::Status::Error)
            escape("unable to create the locus");

          // listen for incoming connections.
          if (elle::Bridge::Listen(locus,
                                   elle::Callback<>::Infer(
                                     &Machine::Connection, this)) ==
              elle::Status::Error)
            escape("unable to listen for bridge connections");
        }

        //
        // set up the gossip timer.
        //
        {
          // create the timer.
          if (this->timer.Create(elle::concurrency::Timer::ModeRepetition) ==
              elle::Status::Error)
            escape("unable to create the timer");

          // subscribe to the timer's signal.
          if (this->timer->signal.timeout.Subscribe(
                elle::Callback<>::Infer(&Machine::Gossip,
                                        this)) == elle::Status::Error)
            escape("unable to subscribe to the signal");

          // start the timer.
          if (this->timer.Start(Machine::Frequency) == elle::Status::Error)
            escape("unable to start the timer");
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
        if (Infinit::Configuration.debug.hole == true)
          std::cout << "[hole] Machine::Put[Immutable]()"
                    << std::endl;

        // check that the machine is authenticated.
        if (this->state != Machine::StateAuthenticated)
          escape("the machine does not seem to have been authenticated");

        /* XXX
        // transfer to the remote.
        if (this->gate->Call(
              elle::network::Inputs<TagPush>(address,
                                    derivable),
              elle::Outputs<elle::TagOk>()) == elle::Status::Error)
          escape("unable to transfer the request");
        */
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
        if (Infinit::Configuration.debug.hole == true)
          std::cout << "[hole] Machine::Put[Mutable]()"
                    << std::endl;

        // check that the machine is authenticated.
        if (this->state != Machine::StateAuthenticated)
          escape("the machine does not seem to have been authenticated");

        /*
        // transfer to the remote.
        if (this->gate->Call(
              elle::network::Inputs<TagPush>(address,
                                    derivable),
              elle::Outputs<elle::TagOk>()) == elle::Status::Error)
          escape("unable to transfer the request");
        */
        return elle::Status::Ok;
      }

      ///
      /// this method retrieves an immutable block.
      ///
      elle::Status      Machine::Get(const nucleus::Address&    address,
                                     nucleus::ImmutableBlock&   block)
      {
        nucleus::Derivable<nucleus::Block>      derivable(block);


        // debug.
        if (Infinit::Configuration.debug.hole == true)
          std::cout << "[hole] Machine::Get[Immutable]()"
                    << std::endl;

        // check that the machine is authenticated.
        if (this->state != Machine::StateAuthenticated)
          escape("the machine does not seem to have been authenticated");

        /*
        // transfer to the remote.
        if (this->gate->Call(
              elle::network::Inputs<TagPull>(address,
                                    nucleus::Version::Any),
              elle::Outputs<TagBlock>(derivable)) == elle::Status::Error)
          escape("unable to transfer the request");
        */
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
        if (Infinit::Configuration.debug.hole == true)
          std::cout << "[hole] Machine::Get[Mutable]()"
                    << std::endl;

        // check that the machine is authenticated.
        if (this->state != Machine::StateAuthenticated)
          escape("the machine does not seem to have been authenticated");

        /*
        // transfer to the remote.
        if (this->gate->Call(
              elle::network::Inputs<TagPull>(address,
                                    version),
              elle::Outputs<TagBlock>(derivable)) == elle::Status::Error)
          escape("unable to transfer the request");
        */
        return elle::Status::Ok;
      }

      ///
      /// this method removes a block.
      ///
      elle::Status      Machine::Kill(const nucleus::Address&   address)
      {

        // debug.
        if (Infinit::Configuration.debug.hole == true)
          std::cout << "[hole] Machine::Kill()"
                    << std::endl;

        // check that the machine is authenticated.
        if (this->state != Machine::StateAuthenticated)
          escape("the machine does not seem to have been authenticated");

        /*
        // transfer to the remote.
        if (this->gate->Call(
              elle::network::Inputs<TagWipe>(address),
              elle::Outputs<elle::TagOk>()) == elle::Status::Error)
          escape("unable to transfer the request");
        */
        return elle::Status::Ok;
      }

//
// ---------- callbacks -------------------------------------------------------
//

      ///
      /// this method handles new connections.
      ///
      elle::Status      Machine::Connection(elle::Gate*                 gate)
      {
        Neighbour*      neighbour;

        enterx(instance(neighbour));

        // debug.
        if (Infinit::Configuration.debug.hole == true)
          std::cout << "[hole] Machine::Connection()"
                    << std::endl;

        // allocate the neighbour.
        neighbour = new Neighbour;

        // create the neighbour.
        if (neighbour->Create(gate) == elle::Status::Error)
          escape("unable to create the neighbour");

        // add the neighbour to the neigbourhood until it gets authenticated.
        //
        // note that the locus here is for sure not the one corresponding
        // to the listening port. this will do it until the neighbour
        // provides us the right port to gossip.
        if (this->neighbourhood.Add(neighbour->locus,
                                    neighbour) == elle::Status::Error)
          escape("unable to add the neighbour to the neigbourhood");

        // challenge the neighbour.
        if (neighbour->gate->Send(
              elle::network::Inputs<TagChallenge>()) == elle::Status::Error)
          escape("unable to send the challenge");

        // waive.
        waive(neighbour);

        return elle::Status::Ok;
      }

      ///
      /// XXX
      ///
      elle::Status      Machine::Challenge()
      {
        Neighbour*      neighbour;
        elle::Session*  session;


        // debug.
        if (Infinit::Configuration.debug.hole == true)
          std::cout << "[hole] Machine::Challenge()"
                    << std::endl;

        // retrieve the network session.
        if (elle::Session::Instance(session) == elle::Status::Error)
          escape("unable to retrieve the current session");

        // retrieve the neighbour from the neigbourhood.
        if (this->neighbourhood.Retrieve(session->locus,
                                         neighbour) == elle::Status::Error)
          escape("unable to retrieve the neighbour");

        // send the passport.
        if (neighbour->gate->Send(
              elle::network::Inputs<TagPassport>(Hole::Passport)) == elle::Status::Error)
          escape("unable to send the passport");

        // also indicate the listening port.
        if (neighbour->gate->Send(
              elle::network::Inputs<TagPort>(this->port)) == elle::Status::Error)
          escape("unable to send the passport");

        return elle::Status::Ok;
      }

      ///
      /// XXX
      ///
      elle::Status      Machine::Passport(const lune::Passport& passport)
      {
        Neighbour*      neighbour;
        elle::Session*  session;


        // debug.
        if (Infinit::Configuration.debug.hole == true)
          std::cout << "[hole] Machine::Passport()"
                    << std::endl;

        // retrieve the network session.
        if (elle::Session::Instance(session) == elle::Status::Error)
          escape("unable to retrieve the current session");

        // retrieve the neighbour from the neighbourhood.
        if (this->neighbourhood.Retrieve(session->locus,
                                         neighbour) == elle::Status::Error)
          escape("unable to retrieve the neighbour");

        // check the neighbour's state, ignore
        if (neighbour->state == Neighbour::StateAuthenticated)
          return elle::Status::Ok;

        // validate the passport.
        if (passport.Validate(Infinit::Authority) == elle::Status::Error)
          escape("unable to validate the passport");

        // set the neighbour's label.
        neighbour->label = passport.label;

        // set the neighbour as authenticated if both the label and the
        // port have been provided.
        if ((neighbour->label != Label::Null) &&
            (neighbour->port != 0))
          {
            // set the state to authenticated.
            neighbour->state = Neighbour::StateAuthenticated;

            // check that the neighbour does not already exist in the
            // routing table i.e with this label.
            if (this->routingtable.Exist(neighbour->label) == elle::Status::True)
              {
                //
                // this case is possible because a neighbour may have
                // been connecting to us, followed by us connecting to it.
                //
                // thus, the machine would reference two instances of the
                // same neighbour.
                //
                // this check make sure to remove the redundant one.
                //

                // remote this neighbour from the neighbourhood.
                if (this->neighbourhood.Remove(
                      neighbour->locus) == elle::Status::Error)
                  escape("unable to remove the neighbour");

                // delete it.
                delete neighbour;

                return elle::Status::Ok;
              }

            // register the neighbour in the routing table.
            if (this->routingtable.Add(neighbour->label,
                                       neighbour) == elle::Status::Error)
              escape("unable to add the neighbour to the routing table");

            // acknowledge the authentication.
            if (neighbour->gate->Reply(
                  elle::network::Inputs<TagAuthenticated>()) == elle::Status::Error)
              escape("unable to acknowledge the authentication");

            // XXX
            this->Dump();
          }

        return elle::Status::Ok;
      }

      ///
      /// XXX
      ///
      elle::Status      Machine::Port(const elle::Port&         port)
      {
        Neighbour*      neighbour;
        elle::Session*  session;


        // debug.
        if (Infinit::Configuration.debug.hole == true)
          std::cout << "[hole] Machine::Port()"
                    << std::endl;

        // retrieve the network session.
        if (elle::Session::Instance(session) == elle::Status::Error)
          escape("unable to retrieve the current session");

        // retrieve the neighbour from the neighbourhood.
        if (this->neighbourhood.Retrieve(session->locus,
                                         neighbour) == elle::Status::Error)
          escape("unable to retrieve the neighbour");

        // check the neighbour's state, ignore
        if (neighbour->state == Neighbour::StateAuthenticated)
          return elle::Status::Ok;

        // set the neighbour's port.
        neighbour->port = port;

        // set the neighbour as authenticated if both the label and the
        // port have been provided.
        if ((neighbour->label != Label::Null) &&
            (neighbour->port != 0))
          {
            // set the state to authenticated.
            neighbour->state = Neighbour::StateAuthenticated;

            // register the neighbour in the routing table.
            if (this->routingtable.Add(neighbour->label,
                                       neighbour) == elle::Status::Error)
              escape("unable to add the neighbour to the routing table");

            // acknowledge the authentication.
            if (neighbour->gate->Reply(
                  elle::network::Inputs<TagAuthenticated>()) == elle::Status::Error)
              escape("unable to acknowledge the authentication");

            // XXX
            this->Dump();
          }

        return elle::Status::Ok;
      }

      ///
      /// XXX
      ///
      elle::Status      Machine::Authenticated()
      {
        Neighbour*      neighbour;
        elle::Session*  session;


        // debug.
        if (Infinit::Configuration.debug.hole == true)
          std::cout << "[hole] Machine::Authenticated()"
                    << std::endl;

        // retrieve the network session.
        if (elle::Session::Instance(session) == elle::Status::Error)
          escape("unable to retrieve the current session");

        // retrieve the neighbour from the neighbourhood.
        if (this->neighbourhood.Retrieve(session->locus,
                                         neighbour) == elle::Status::Error)
          escape("unable to retrieve the neighbour");

        // set the machine as being authenticated.
        this->state = Machine::StateAuthenticated;

        return elle::Status::Ok;
      }

      ///
      /// XXX
      ///
      elle::Status      Machine::Update(const Cluster&          cluster)
      {
        Neighbour*              neighbour;
        elle::Session*          session;
        Cluster::Scoutor        scoutor;


        // debug.
        if (Infinit::Configuration.debug.hole == true)
          std::cout << "[hole] Machine::Update()"
                    << std::endl;

        // retrieve the network session.
        if (elle::Session::Instance(session) == elle::Status::Error)
          escape("unable to retrieve the current session");

        // retrieve the neighbour from the neighbourhood.
        if (this->neighbourhood.Retrieve(session->locus,
                                         neighbour) == elle::Status::Error)
          escape("unable to retrieve the neighbour");

        // check that the machine has been authenticated.
        if (this->state != Machine::StateAuthenticated)
          escape("the machine has not been authenticated yet");

        // check that the neighbour has been authenticated.
        if (neighbour->state != Neighbour::StateAuthenticated)
          escape("unable to process gossip from unauthenticated neighbours");

        // go through the cluster.
        for (scoutor = cluster.container.begin();
             scoutor != cluster.container.end();
             scoutor++)
          {
            elle::network::Locus         locus = *scoutor;
            Neighbour*          neighbour;

            enterx(instance(neighbour));

            // check if this locus is already registered.
            if (this->neighbourhood.Exist(locus) == elle::Status::True)
              continue;

            // allocate the neighbour.
            neighbour = new Neighbour;

            // create the neighbour.
            if (neighbour->Create(locus) == elle::Status::Error)
              escape("unable to create the neighbour");

            // add the neighbour to the neighbourhood.
            if (this->neighbourhood.Add(neighbour->locus,
                                        neighbour) == elle::Status::Error)
              escape("unable to add the neighbour to the neighbourhood");

            // connect the neighbour.
            if (neighbour->Connect() == elle::Status::Error)
              escape("unable to connect the neighbour");

            // waive.
            waive(neighbour);
          }

        return elle::Status::Ok;
      }

      ///
      /// XXX
      ///
      elle::Status      Machine::Gossip()
      {
        Cluster                 cluster;
        RoutingTable::Scoutor   scoutor;

        // debug.
        if (Infinit::Configuration.debug.hole == true)
          std::cout << "[hole] Machine::Gossip()"
                    << std::endl;

        // check that the machine has been authenticated.
        if (this->state != Machine::StateAuthenticated)
          escape("the machine has not been authenticated yet");

        // create the cluster related to the current routing table.
        if (cluster.Create(this->routingtable) == elle::Status::Error)
          escape("unable to create the cluster");

        // go through the routing table entries.
        for (scoutor = this->routingtable.container.begin();
             scoutor != this->routingtable.container.end();
             scoutor++)
          {
            Neighbour*          neighbour = scoutor->second;

            // send the cluster to the neighbour.
            if (neighbour->gate->Send(
                  elle::network::Inputs<TagUpdate>(cluster)) == elle::Status::Error)
              escape("unable to send the cluster");
          }

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

        // dump the neighbourhood.
        if (this->neighbourhood.Dump(margin + 2) == elle::Status::Error)
          escape("unable to dump the neighbourhood");

        // dump the routing table.
        if (this->routingtable.Dump(margin + 2) == elle::Status::Error)
          escape("unable to dump the routing table");

        // dump the timer.
        if (this->timer.Dump(margin + 2) == elle::Status::Error)
          escape("unable to dump the timer");

        // dump the port.
        std::cout << alignment << elle::Dumpable::Shift
                  << "[Port] " << std::dec << this->port << std::endl;

        return elle::Status::Ok;
      }

    }
  }
}
