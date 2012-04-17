//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [thu may 26 09:58:52 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/implementations/remote/Server.hh>
#include <hole/implementations/remote/Manifest.hh>

#include <hole/Hole.hh>

#include <lune/Lune.hh>

#include <Infinit.hh>

namespace hole
{
  namespace implementations
  {
    namespace remote
    {

//
// ---------- constructors & destructors --------------------------------------
//

      ///
      /// default constructor.
      ///
      Server::Server(const elle::network::Locus&                         locus):
        locus(locus)
      {
      }

      ///
      /// destructor.
      ///
      Server::~Server()
      {
        Server::Scoutor scoutor;

        // go though the customer.
        for (scoutor = this->container.begin();
             scoutor != this->container.end();
             scoutor++)
          {
            Customer*   customer = scoutor->second;

            // ignore dead customers.
            if (customer->state == Customer::StateDead)
              continue;

            // delete the customer.
            delete customer;
          }

        // clear the container.
        this->container.clear();
      }

//
// ---------- methods ---------------------------------------------------------
//

      ///
      /// launch the server by waiting for incoming connections.
      ///
      elle::Status      Server::Launch()
      {

        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::remote::Server::Launch()\n");

        //
        // register the messages.
        //
        {
          // register the challenge message.
          if (elle::Network::Register(
                elle::Procedure<TagChallenge,
                                elle::TagNone,
                                TagException>(
                  elle::Callback<>::Infer(
                    &Server::Challenge, this))) == elle::Status::Error)
            escape("unable to register the callback");

          // register the push message.
          if (elle::Network::Register(
                elle::Procedure<TagPush,
                                elle::TagNone,
                                TagException>(
                  elle::Callback<>::Infer(
                    &Server::Push, this))) == elle::Status::Error)
            escape("unable to register the callback");

          // register the pull message.
          if (elle::Network::Register(
                elle::Procedure<TagPull,
                                elle::TagNone,
                                TagException>(
                  elle::Callback<>::Infer(
                    &Server::Pull, this))) == elle::Status::Error)
            escape("unable to register the callback");

          // register the wipe message.
          if (elle::Network::Register(
                elle::Procedure<TagWipe,
                                elle::TagNone,
                                TagException>(
                  elle::Callback<>::Infer(
                    &Server::Wipe, this))) == elle::Status::Error)
            escape("unable to register the callback");
        }

        //
        // create the connection.
        //
        {
          // listen for incoming connections.
          if (elle::TCPServer::Listen(
                this->locus,
                elle::Callback<>::Infer(
                  &Server::Connection, this)) == elle::Status::Error)
            escape("unable to listen for TCP connections");
        }

        return elle::Status::Ok;
      }

      ///
      /// this method adds the given customer to the set of customers.
      ///
      elle::Status      Server::Add(elle::network::TCPSocket*            socket,
                                    Customer*                   customer)
      {
        std::pair<Server::Iterator, elle::Boolean>      result;


        // check if this customer already exists.
        if (this->Locate(socket) == elle::Status::True)
          escape("this socket has already been registered");

        // insert the customer in the container.
        result = this->container.insert(
                   std::pair<elle::network::TCPSocket*, Customer*>(socket, customer));

        // check if the insertion was successful.
        if (result.second == false)
          escape("unable to insert the customer in the container");

        return elle::Status::Ok;
      }

      ///
      /// this method removes a customer from the set.
      ///
      elle::Status      Server::Remove(elle::network::TCPSocket*         socket)
      {
        Server::Iterator        iterator;


        // locate the customer.
        if (this->Locate(socket, &iterator) == elle::Status::False)
          escape("unable to locate the given customer");

        // remove the entry from the container.
        this->container.erase(iterator);

        return elle::Status::Ok;
      }

      ///
      /// this method returns the customer associated with the given socket.
      ///
      elle::Status      Server::Retrieve(elle::network::TCPSocket*       socket,
                                         Customer*&             customer)
      {
        Server::Iterator        iterator;


        // locate the customer.
        if (this->Locate(socket, &iterator) == elle::Status::False)
          escape("unable to locate the given customer");

        // retrieve the customer.
        customer = iterator->second;

        return elle::Status::Ok;
      }

      ///
      /// this method locates the customer associated with the given socket.
      ///
      elle::Status      Server::Locate(elle::network::TCPSocket*         socket,
                                       Iterator*                iterator)
      {
        Server::Iterator        i;


        // try to locate the customer.
        if ((i = this->container.find(socket)) != this->container.end())
          {
            if (iterator != NULL)
              *iterator = i;

            return elle::Status::True;
          }

        return elle::Status::False;
      }

      ///
      /// this method stores an immutable block.
      ///
      elle::Status      Server::Put(const nucleus::Address&     address,
                                    const nucleus::ImmutableBlock& block)
      {

        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::remote::Server::Put[Immutable]()\n");

        // does the block already exist.
        if (block.Exist(Hole::Implementation->network,
                        address) == elle::Status::True)
          escape("this immutable block seems to already exist");

        // store the block.
        if (block.Store(Hole::Implementation->network,
                        address) == elle::Status::Error)
          escape("unable to store the block");

        return elle::Status::Ok;
      }

      ///
      /// this method stores a mutable block.
      ///
      elle::Status      Server::Put(const nucleus::Address&     address,
                                    const nucleus::MutableBlock& block)
      {

        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::remote::Server::Put[Mutable]()\n");

        // validate the block, depending on its component.
        //
        // indeed, the Object component requires as additional block for
        // being validated.
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
                  if (object->Validate(address, access) == elle::Status::Error)
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
              escape("unknown component '%u'", address.component);
            }
          }

        // store the block.
        if (block.Store(Hole::Implementation->network,
                        address) == elle::Status::Error)
          escape("unable to store the block");

        return elle::Status::Ok;
      }

      ///
      /// this method retrieves an immutable block.
      ///
      elle::Status      Server::Get(const nucleus::Address&     address,
                                    nucleus::ImmutableBlock&    block)
      {

        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::remote::Server::Get[Immutable]()\n");

        // does the block exist.
        if (block.Exist(Hole::Implementation->network,
                        address) == elle::Status::False)
          escape("the block does not seem to exist");

        // load the block.
        if (block.Load(Hole::Implementation->network,
                       address) == elle::Status::Error)
          escape("unable to load the block");

        // validate the block.
        if (block.Validate(address) == elle::Status::Error)
          escape("the block seems to be invalid");

        return elle::Status::Ok;
      }

      ///
      /// this method retrieves a mutable block.
      ///
      elle::Status      Server::Get(const nucleus::Address&     address,
                                    const nucleus::Version&     version,
                                    nucleus::MutableBlock&      block)
      {
        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::remote::Server::Get[Mutable]()\n");

        // does the block exist.
        if (block.Exist(Hole::Implementation->network,
                        address, version) == elle::Status::False)
          escape("the block does not seem to exist");

        // load the block.
        if (block.Load(Hole::Implementation->network,
                       address, version) == elle::Status::Error)
          escape("unable to load the block");

        // validate the block, depending on its component.
        //
        // indeed, the Object component requires as additional block for
        // being validated.
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
                  if (object->Validate(address, access) == elle::Status::Error)
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

        return elle::Status::Ok;
      }

      ///
      /// this method removes a block.
      ///
      elle::Status      Server::Kill(const nucleus::Address&    address)
      {
        nucleus::Block  block;


        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::remote::Server::Kill()\n");

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

        return elle::Status::Ok;
      }

//
// ---------- callbacks -------------------------------------------------------
//

      ///
      /// this callback handles new connections.
      ///
      elle::Status      Server::Connection(elle::network::TCPSocket*     socket)
      {
        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::remote::Server::Connection()\n");

        // allocate a customer.
        auto customer = std::unique_ptr<Customer>(new Customer);

        // create the customer.
        if (customer->Create(socket) == elle::Status::Error)
          escape("unable to create the customer");

        // set the state.
        customer->state = Customer::StateConnected;

        // subscribe to the signal.
        if (customer->signal.dead.Subscribe(
              elle::Callback<>::Infer(
                &Server::Sweep, this
              )
            ) == elle::Status::Error)
          escape("unable to subscribe to the signal");

        // add the customer.
        if (this->Add(customer->socket, customer.get()) == elle::Status::Error)
          escape("unable to add the customer");
        else
          customer.release();

        return elle::Status::Ok;
      }

      ///
      /// this callback is triggered whenever the client initiate the
      /// authentication challenge.
      ///
      elle::Status      Server::Challenge(const lune::Passport& passport)
      {
        Customer*       customer;
        elle::Session*  session;


        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::remote::Server::Challenge()\n");

        // retrieve the network session.
        if (elle::Session::Instance(session) == elle::Status::Error)
          escape("unable to retrieve the current session");

        // retrieve the customer.
        if (this->Retrieve(dynamic_cast<elle::network::TCPSocket*>(session->socket),
                           customer) == elle::Status::Error)
          escape("unable to retrieve the customer");

        // validate the passport.
        if (passport.Validate(Infinit::Authority) == elle::Status::Error)
          {
            // remove the customer.
            if (this->Remove(customer->socket) == elle::Status::Error)
              escape("unable to remove the customer");

            // disconnect the customer.
            if (customer->socket->Disconnect() == elle::Status::Error)
              escape("unable to disconnect the customer");

            // bury the customer: the socket is still in use.
            bury(customer);
          }
        else
          {
            // set the state as authenticated.
            customer->state = Customer::StateAuthenticated;

            // reply with the authenticated message.
            if (customer->socket->Reply(
                  elle::Inputs<TagAuthenticated>()) == elle::Status::Error)
              escape("unable to reply to the client");
          }

        return elle::Status::Ok;
      }

      ///
      /// this callback is triggered whenever a customer dies.
      ///
      elle::Status      Server::Sweep(Customer*                 customer)
      {

        // remove the customer.
        if (this->Remove(customer->socket) == elle::Status::Error)
          escape("unable to remove the customer");

        // bury it.
        bury(customer);

        return elle::Status::Ok;
      }

      ///
      /// this method stores the given block.
      ///
      elle::Status      Server::Push(const nucleus::Address&    address,
                                     const
                                       nucleus::Derivable
                                         <nucleus::Block>&      derivable)
      {
        Customer*       customer;
        elle::Session*  session;
        nucleus::Block* object;


        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::remote::Server::Push()\n");

        // retrieve the network session.
        if (elle::Session::Instance(session) == elle::Status::Error)
          escape("unable to retrieve the current session");

        // retrieve the customer.
        if (this->Retrieve(dynamic_cast<elle::network::TCPSocket*>(session->socket),
                           customer) == elle::Status::Error)
          escape("unable to retrieve the customer");

        // check that the client has been authenticated.
        if (customer->state != Customer::StateAuthenticated)
          escape("the customer has not been authenticated");

        // infer the block from the derivable.
        if (derivable.Infer(object) == elle::Status::Error)
          escape("unable to infer the block from the derivable");

        // forward the request depending on the nature of the block which
        // the addres indicates.
        switch (address.family)
          {
          case nucleus::FamilyContentHashBlock:
            {
              nucleus::ImmutableBlock*  ib;

              // cast to an immutable block.
              ib = static_cast<nucleus::ImmutableBlock*>(object);

              // store the immutable block.
              if (this->Put(address, *ib) == elle::Status::Error)
                escape("unable to put the block");

              break;
            }
          case nucleus::FamilyPublicKeyBlock:
          case nucleus::FamilyOwnerKeyBlock:
          case nucleus::FamilyImprintBlock:
            {
              nucleus::MutableBlock*    mb;

              // cast to a mutable block.
              mb = static_cast<nucleus::MutableBlock*>(object);

              // store the mutable block.
              if (this->Put(address, *mb) == elle::Status::Error)
                escape("unable to put the block");

              break;
            }
          default:
            {
              escape("unknown block family");
            }
          }

        // acknowledge.
        if (customer->socket->Reply(
              elle::Inputs<elle::TagOk>()) == elle::Status::Error)
          escape("unable to acknowledge");

        return elle::Status::Ok;
      }

      ///
      /// this method returns the block associated with the given address.
      ///
      elle::Status      Server::Pull(const nucleus::Address&    address,
                                     const nucleus::Version&    version)
      {
        Customer*       customer;
        elle::Session*  session;
        nucleus::Block* block;

        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::remote::Server::Pull()\n");

        // retrieve the network session.
        if (elle::Session::Instance(session) == elle::Status::Error)
          escape("unable to retrieve the current session");

        // retrieve the customer.
        if (this->Retrieve(dynamic_cast<elle::network::TCPSocket*>(session->socket),
                           customer) == elle::Status::Error)
          escape("unable to retrieve the customer");

        // check that the client has been authenticated.
        if (customer->state != Customer::StateAuthenticated)
          escape("the customer has not been authenticated");

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

              // retrieve the immutable block.
              if (this->Get(address, *ib) == elle::Status::Error)
                escape("unable to get the block");

              break;
            }
          case nucleus::FamilyPublicKeyBlock:
          case nucleus::FamilyOwnerKeyBlock:
          case nucleus::FamilyImprintBlock:
            {
              nucleus::MutableBlock*    mb;

              // cast to a mutable block.
              mb = static_cast<nucleus::MutableBlock*>(block);

              // retrieve the mutable block.
              if (this->Get(address, version,
                            *mb) == elle::Status::Error)
                escape("unable to get the block");

              break;
            }
          default:
            {
              escape("unknown block family");
            }
          }

        nucleus::Derivable<nucleus::Block> derivable(address.component, *block);

        // return the block.
        if (customer->socket->Reply(
              elle::Inputs<TagBlock>(derivable)) == elle::Status::Error)
          escape("unable to return the block");

        return elle::Status::Ok;
      }

      ///
      /// this method removes the block associated with the given address.
      ///
      elle::Status      Server::Wipe(const nucleus::Address&    address)
      {
        Customer*       customer;
        elle::Session*  session;

        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::remote::Server::Wipe()\n");

        // retrieve the network session.
        if (elle::Session::Instance(session) == elle::Status::Error)
          escape("unable to retrieve the current session");

        // retrieve the customer.
        if (this->Retrieve(dynamic_cast<elle::network::TCPSocket*>(session->socket),
                           customer) == elle::Status::Error)
          escape("unable to retrieve the customer");

        // check that the client has been authenticated.
        if (customer->state != Customer::StateAuthenticated)
          escape("the customer has not been authenticated");

        // forward the kill request to the implementation.
        if (this->Kill(address) == elle::Status::Error)
          escape("unable to erase the block");

        // acknowledge.
        if (customer->socket->Reply(
              elle::Inputs<elle::TagOk>()) == elle::Status::Error)
          escape("unable to acknowledge");

        return elle::Status::Ok;
      }

//
// ---------- dumpable --------------------------------------------------------
//

      ///
      /// this method dumps the server.
      ///
      elle::Status      Server::Dump(const elle::Natural32      margin) const
      {
        elle::String    alignment(margin, ' ');
        Server::Scoutor scoutor;

        std::cout << alignment << "[Server]" << std::endl;

        // dump the locus.
        if (this->locus.Dump(margin + 2) == elle::Status::Error)
          escape("unable to dump the locus");

        std::cout << alignment << elle::Dumpable::Shift
                  << "[Customers] " << this->container.size() << std::endl;

        // go though the customer.
        for (scoutor = this->container.begin();
             scoutor != this->container.end();
             scoutor++)
          {
            Customer*   customer = scoutor->second;

            // dump the customer.
            if (customer->Dump(margin + 4) == elle::Status::Error)
              escape("unable to dump the customer");
          }

        return elle::Status::Ok;
      }

    }
  }
}
