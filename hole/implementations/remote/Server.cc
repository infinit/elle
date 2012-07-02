#include <elle/log.hh>
#include <elle/network/Network.hh>
#include <elle/network/Bundle.hh>
#include <elle/standalone/Report.hh>
#include <elle/utility/Buffer.hh>
#include <elle/network/Header.hh>
#include <elle/standalone/Morgue.hh>
#include <elle/concurrency/Callback.hh>

#include <reactor/network/tcp-server.hh>

#include <nucleus/proton/Block.hh>
#include <nucleus/proton/Block.hh>

#include <hole/Hole.hh>
#include <hole/implementations/remote/Manifest.hh>
#include <hole/implementations/remote/Server.hh>

#include <lune/Lune.hh>

#include <Infinit.hh>

ELLE_LOG_TRACE_COMPONENT("Infinit.Hole.Remote.Server");

namespace hole
{
  namespace implementations
  {
    namespace remote
    {
      /*-------------.
      | Construction |
      `-------------*/

      Server::Server(const elle::network::Locus&         locus)
        : _locus(locus)
        , _server(new reactor::network::TCPServer
                  (elle::concurrency::scheduler()))
        , _acceptor(0)
      {
        this->_server->listen(locus.port);
      }

      Server::~Server()
      {
        this->_acceptor->terminate_now();
        delete this->_acceptor;
        this->_acceptor = 0;
        Server::Scoutor scoutor;
        for (scoutor = this->container.begin();
             scoutor != this->container.end();
             scoutor++)
          {
            Customer*   customer = scoutor->second;
            // ignore dead customers.
            if (customer->state == Customer::StateDead)
              continue;
            delete customer;
          }
        this->container.clear();
        delete this->_server;
        this->_server = nullptr;
      }

      ///
      /// launch the server by waiting for incoming connections.
      ///
      elle::Status      Server::Launch()
      {
        ELLE_LOG_TRACE_SCOPE("Launch");

        //
        // register the messages.
        //
        {
          // register the challenge message.
          if (elle::network::Network::Register(
                elle::network::Procedure<TagChallenge,
                                         elle::TagNone,
                                         elle::TagError>(
                  elle::concurrency::Callback<>::Infer(
                    &Server::Challenge, this))) == elle::Status::Error)
            escape("unable to register the callback");

          // register the push message.
          if (elle::network::Network::Register(
                elle::network::Procedure<TagPush,
                                         elle::TagNone,
                                         elle::TagError>(
                  elle::concurrency::Callback<>::Infer(
                    &Server::Push, this))) == elle::Status::Error)
            escape("unable to register the callback");

          // register the pull message.
          if (elle::network::Network::Register(
                elle::network::Procedure<TagPull,
                                         elle::TagNone,
                                         elle::TagError>(
                  elle::concurrency::Callback<>::Infer(
                    &Server::Pull, this))) == elle::Status::Error)
            escape("unable to register the callback");

          // register the wipe message.
          if (elle::network::Network::Register(
                elle::network::Procedure<TagWipe,
                                         elle::TagNone,
                                         elle::TagError>(
                  elle::concurrency::Callback<>::Infer(
                    &Server::Wipe, this))) == elle::Status::Error)
            escape("unable to register the callback");
        }

        _acceptor = new reactor::Thread(elle::concurrency::scheduler(),
                                        "Remote Server accept",
                                        boost::bind(&Server::_accept, this));
        return elle::Status::Ok;
      }

      void
      Server::_accept()
      {
        while (true)
          {
            reactor::network::TCPSocket* socket = _server->accept();

            ELLE_LOG_TRACE_SCOPE("accept");

            auto customer = new Customer(new elle::network::TCPSocket(socket));

            // set the state.
            // FIXME: this status should be handled by customer itself
            customer->state = Customer::StateConnected;

            // subscribe to the signal.
            if (customer->signal.dead.Subscribe(
                  elle::concurrency::Callback<>::Infer(
                    &Server::Sweep, this
                    )
                  ) == elle::Status::Error)
              // FIXME
              // escape("unable to subscribe to the signal");
              std::abort();

            // add the customer.
            if (this->Add(customer->socket, customer) == elle::Status::Error)
              // FIXME
              // escape("unable to add the customer");
              std::abort();
          }
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
        ELLE_LOG_TRACE_SCOPE("Put[Immutable]");

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
        ELLE_LOG_TRACE_SCOPE("Put[Mutable]");

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
        ELLE_LOG_TRACE_SCOPE("Get[Immutable]");

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
        ELLE_LOG_TRACE_SCOPE("Get[Mutable]");

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
        ELLE_LOG_TRACE_SCOPE("Kill");

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

        return elle::Status::Ok;
      }

//
// ---------- callbacks -------------------------------------------------------
//

      ///
      /// this callback is triggered whenever the client initiate the
      /// authentication challenge.
      ///
      elle::Status      Server::Challenge(const lune::Passport& passport)
      {
        Customer*       customer;

        ELLE_LOG_TRACE_SCOPE("Challenge");

        // retrieve the customer.
        if (this->Retrieve(elle::network::current_context().socket, customer) == elle::Status::Error)
          escape("unable to retrieve the customer");

        // validate the passport.
        if (passport.Validate(Infinit::Authority) == elle::Status::Error)
          {
            // remove the customer.
            if (this->Remove(customer->socket) == elle::Status::Error)
              escape("unable to remove the customer");

            // disconnect the customer.
            customer->socket->Disconnect();

            // bury the customer: the socket is still in use.
            bury(customer);
          }
        else
          {
            // set the state as authenticated.
            customer->state = Customer::StateAuthenticated;

            // reply with the authenticated message.
            if (customer->socket->Reply(
                  elle::network::Inputs<TagAuthenticated>()) == elle::Status::Error)
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
      elle::Status      Server::Push(nucleus::Address const&    address,
                                     nucleus::Derivable const&  derivable)
      {
        Customer*       customer;

        ELLE_LOG_TRACE_SCOPE("Push");

        auto const& block =
          static_cast<nucleus::Derivable const&>(derivable).block();

        // retrieve the customer.
        if (this->Retrieve(elle::network::current_context().socket,
                           customer) == elle::Status::Error)
          escape("unable to retrieve the customer");

        // check that the client has been authenticated.
        if (customer->state != Customer::StateAuthenticated)
          escape("the customer has not been authenticated");

        // forward the request depending on the nature of the block which
        // the addres indicates.
        switch (address.family)
          {
          case nucleus::FamilyContentHashBlock:
            {
              nucleus::ImmutableBlock const& ib =
                static_cast<nucleus::ImmutableBlock const&>(block);

              // store the immutable block.
              if (this->Put(address, ib) == elle::Status::Error)
                escape("unable to put the block");

              break;
            }
          case nucleus::FamilyPublicKeyBlock:
          case nucleus::FamilyOwnerKeyBlock:
          case nucleus::FamilyImprintBlock:
            {
              nucleus::MutableBlock const&  mb =
                static_cast<nucleus::MutableBlock const&>(block);
              assert(dynamic_cast<nucleus::MutableBlock const*>(&block));

              // store the mutable block.
              if (this->Put(address, mb) == elle::Status::Error)
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
              elle::network::Inputs<elle::TagOk>()) == elle::Status::Error)
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
        nucleus::Block* block;

        ELLE_LOG_TRACE_SCOPE("Pull");

        // retrieve the customer.
        if (this->Retrieve(elle::network::current_context().socket,
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
              assert(dynamic_cast<nucleus::MutableBlock*>(block));


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

        nucleus::Derivable derivable(address.component, *block);

        // return the block.
        if (customer->socket->Reply(
              elle::network::Inputs<TagBlock>(derivable)) == elle::Status::Error)
          escape("unable to return the block");

        return elle::Status::Ok;
      }

      ///
      /// this method removes the block associated with the given address.
      ///
      elle::Status      Server::Wipe(const nucleus::Address&    address)
      {
        Customer*       customer;

        ELLE_LOG_TRACE_SCOPE("Wipe");

        // retrieve the customer.
        if (this->Retrieve(elle::network::current_context().socket,
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
              elle::network::Inputs<elle::TagOk>()) == elle::Status::Error)
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
        if (this->_locus.Dump(margin + 2) == elle::Status::Error)
          escape("unable to dump the locus");

        std::cout << alignment << elle::io::Dumpable::Shift
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
