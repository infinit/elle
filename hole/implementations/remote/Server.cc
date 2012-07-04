#include <hole/Hole.hh>
#include <hole/Holeable.hh>
#include <hole/implementations/remote/Manifest.hh>
#include <hole/implementations/remote/Server.hh>

#include <elle/log.hh>
#include <elle/network/Network.hh>
#include <elle/standalone/Morgue.hh>
#include <elle/concurrency/Callback.hh>

#include <reactor/network/tcp-server.hh>

#include <nucleus/proton/ImmutableBlock.hh>
#include <nucleus/proton/MutableBlock.hh>
#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Network.hh>
#include <nucleus/proton/Version.hh>
#include <nucleus/neutron/Component.hh>
#include <nucleus/neutron/Object.hh>
#include <nucleus/neutron/Access.hh>
#include <nucleus/Derivable.hh>

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

            auto customer = new Customer(new elle::network::TCPSocket(socket, false));

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
            if (iterator != nullptr)
              *iterator = i;

            return elle::Status::True;
          }

        return elle::Status::False;
      }

      ///
      /// this method stores an immutable block.
      ///
      elle::Status      Server::Put(const nucleus::proton::Address& address,
                                    const nucleus::proton::ImmutableBlock& block)
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
      elle::Status      Server::Put(const nucleus::proton::Address& address,
                                    const nucleus::proton::MutableBlock& block)
      {
        ELLE_LOG_TRACE_SCOPE("Put[Mutable]");

        // validate the block, depending on its component.
        //
        // indeed, the Object component requires as additional block for
        // being validated.
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
                  std::unique_ptr<nucleus::proton::Block> block
                    (Hole::Pull(object->meta.access, nucleus::proton::Version::Last));
                  std::unique_ptr<nucleus::neutron::Access> access
                    (dynamic_cast<nucleus::neutron::Access*>(block.release()));
                  if (access == nullptr)
                    throw reactor::Exception(elle::concurrency::scheduler(),
                                             "expected an access block");

                  // validate the object, providing the
                  if (object->Validate(address, *access) == elle::Status::Error)
                    escape("unable to validate the object");
                }
              else
                {
                  // validate the object.
                  if (object->Validate(
                        address,
                        nucleus::neutron::Access::Null) == elle::Status::Error)
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
          case nucleus::neutron::ComponentUnknown:
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
      elle::Status      Server::Get(const nucleus::proton::Address& address,
                                    nucleus::proton::ImmutableBlock& block)
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
      elle::Status      Server::Get(const nucleus::proton::Address& address,
                                    const nucleus::proton::Version& version,
                                    nucleus::proton::MutableBlock& block)
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
                  if (!access)
                    throw reactor::Exception(elle::concurrency::scheduler(),
                                             "expected an access block");

                  // validate the object, providing the
                  if (object->Validate(address, *access) == elle::Status::Error)
                    escape("unable to validate the object");
                }
              else
                {
                  // validate the object.
                  if (object->Validate(
                        address,
                        nucleus::neutron::Access::Null) == elle::Status::Error)
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
          case nucleus::neutron::ComponentUnknown:
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
      elle::Status      Server::Kill(const nucleus::proton::Address& address)
      {
        ELLE_LOG_TRACE_SCOPE("Kill");

        // treat the request depending on the nature of the block which
        // the addres indicates.
        switch (address.family)
          {
          case nucleus::proton::FamilyContentHashBlock:
            {
              // erase the immutable block.
              if (nucleus::proton::ImmutableBlock::Erase(Hole::Implementation->network,
                           address) == elle::Status::Error)
                escape("unable to erase the block");

              break;
            }
          case nucleus::proton::FamilyPublicKeyBlock:
          case nucleus::proton::FamilyOwnerKeyBlock:
          case nucleus::proton::FamilyImprintBlock:
            {
              // retrieve the mutable block.
              if (nucleus::proton::MutableBlock::Erase(Hole::Implementation->network,
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

      elle::Status
      Server::Challenge(const lune::Passport& passport)
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
            customer->state = Customer::StateAuthenticated;
            customer->socket->reply(elle::network::Inputs<TagAuthenticated>());
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
      elle::Status      Server::Push(nucleus::proton::Address const& address,
                                     nucleus::Derivable const& derivable)
      {
        Customer*       customer;

        ELLE_LOG_TRACE_SCOPE("Push");

        auto const& block = derivable.block();

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
          case nucleus::proton::FamilyContentHashBlock:
            {
              nucleus::proton::ImmutableBlock const* ib =
                static_cast<nucleus::proton::ImmutableBlock const*>(&block);
              assert(dynamic_cast<const nucleus::proton::ImmutableBlock*>(
                       &block) != nullptr);

              // store the immutable block.
              if (this->Put(address, *ib) == elle::Status::Error)
                escape("unable to put the block");

              break;
            }
          case nucleus::proton::FamilyPublicKeyBlock:
          case nucleus::proton::FamilyOwnerKeyBlock:
          case nucleus::proton::FamilyImprintBlock:
            {
              nucleus::proton::MutableBlock const*  mb =
                static_cast<nucleus::proton::MutableBlock const*>(&block);
              assert(dynamic_cast<nucleus::proton::MutableBlock const*>(&block)
                     != nullptr);

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

        // Acknowledge.
        customer->socket->reply(elle::network::Inputs<elle::TagOk>());

        return elle::Status::Ok;
      }

      ///
      /// this method returns the block associated with the given address.
      ///
      elle::Status      Server::Pull(const nucleus::proton::Address& address,
                                     const nucleus::proton::Version& version)
      {
        Customer*       customer;
        nucleus::proton::Block* block;

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

        std::unique_ptr<nucleus::proton::Block> guard(block);

        // forward the request depending on the nature of the block which
        // the addres indicates.
        switch (address.family)
          {
          case nucleus::proton::FamilyContentHashBlock:
            {
              nucleus::proton::ImmutableBlock* ib;

              // cast to an immutable block.
              ib = static_cast<nucleus::proton::ImmutableBlock*>(block);
              assert(dynamic_cast<nucleus::proton::ImmutableBlock*>(
                       block) != nullptr);

              // retrieve the immutable block.
              if (this->Get(address, *ib) == elle::Status::Error)
                escape("unable to get the block");

              break;
            }
          case nucleus::proton::FamilyPublicKeyBlock:
          case nucleus::proton::FamilyOwnerKeyBlock:
          case nucleus::proton::FamilyImprintBlock:
            {
              nucleus::proton::MutableBlock* mb;

              // cast to a mutable block.
              mb = static_cast<nucleus::proton::MutableBlock*>(block);
              assert(dynamic_cast<nucleus::proton::MutableBlock*>(
                       block) != nullptr);

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

        // Return the block.
        customer->socket->reply(elle::network::Inputs<TagBlock>(derivable));

        return elle::Status::Ok;
      }

      ///
      /// this method removes the block associated with the given address.
      ///
      elle::Status      Server::Wipe(const nucleus::proton::Address& address)
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
        customer->socket->reply(elle::network::Inputs<elle::TagOk>());

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
