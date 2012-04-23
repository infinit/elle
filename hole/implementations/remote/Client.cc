
#include <elle/standalone/ReportSerializer.hxx>
#include <elle/network/BundleSerializer.hxx>
#include <elle/utility/BufferSerializer.hxx>
#include <elle/network/HeaderSerializer.hxx>

#include <elle/concurrency/Program.hh>
#include <elle/network/Locus.hh>
#include <elle/network/Range.hh>
#include <elle/network/TCPSocket.hh>
#include <elle/utility/Time.hh>
#include <elle/network/Network.hh>

#include <hole/implementations/remote/Client.hh>
#include <hole/implementations/remote/Manifest.hh>

#include <hole/Hole.hh>

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
      Client::Client(const elle::network::Locus&                         locus):
        state(Client::StateUnknown),
        locus(locus),
        socket(NULL)
      {
      }

      ///
      /// the destructor.
      ///
      Client::~Client()
      {
        // delete the socket.
        if (this->socket != NULL)
        {
          delete this->socket;
          this->socket = 0;
        }
      }

//
// ---------- peer ------------------------------------------------------------
//

      ///
      /// this method launches the client.
      ///
      elle::Status      Client::Launch()
      {
        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::remote::Client::Launch()\n");

        //
        // register the messages.
        //
        {
          // register the message.
          if (elle::network::Network::Register(
                elle::network::Procedure<TagAuthenticated>(
                  elle::concurrency::Callback<>::Infer(
                    &Client::Authenticated, this))) == elle::Status::Error)
            escape("unable to register the callback");

          // register the message.
          if (elle::network::Network::Register(
                elle::network::Procedure<TagException>(
                  elle::concurrency::Callback<>::Infer(
                    &Client::Exception, this))) == elle::Status::Error)
            escape("unable to register the callback");
        }

        //
        // connect to the server.
        //
        {
          // allocate the socket.
          this->socket = new elle::network::TCPSocket;

          // create the socket.
          if (this->socket->Create() == elle::Status::Error)
            escape("unable to create the socket");

          // subscribe to the signal.
          if (this->socket->signal.connected.Subscribe(
                elle::concurrency::Callback<>::Infer(&Client::Connected,
                                        this)) == elle::Status::Error)
            escape("unable to subscribe to the signal");

          // subscribe to the signal.
          if (this->socket->signal.disconnected.Subscribe(
                elle::concurrency::Callback<>::Infer(&Client::Disconnected,
                                        this)) == elle::Status::Error)
            escape("unable to subscribe to the signal");

          // subscribe to the signal.
          if (this->socket->signal.error.Subscribe(
                elle::concurrency::Callback<>::Infer(&Client::Error,
                                        this)) == elle::Status::Error)
            escape("unable to subscribe to the signal");

          // connect the socket.
          if (this->socket->Connect(this->locus,
                                    elle::network::AbstractSocket::ModeSynchronous) ==
              elle::Status::Error)
            escape("unable to connect to the bridge");
        }

        //
        // authenticate to the server.
        //
        {
          // send the passport.
          if (this->socket->Send(
                elle::network::Inputs<TagChallenge>(Hole::Passport)) ==
              elle::Status::Error)
            escape("unable to send the challenge");
        }

        return elle::Status::Ok;
      }

      ///
      /// this method stores an immutable block.
      ///
      elle::Status      Client::Put(const nucleus::Address&     address,
                                    const nucleus::ImmutableBlock& block)
      {
        //nucleus::Derivable<nucleus::Block>      derivable(address.component,
        //                                                  block);

        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::remote::Client::Put[Immutable]()\n");

        // check that the client is connected.
        if (this->socket == NULL)
          escape("the client seems to have been disconnected");

        // transfer to the remote.
        if (this->socket->Call(
              elle::network::Inputs<TagPush>(address, block),
              elle::network::Outputs<elle::TagOk>()) == elle::Status::Error)
          escape("unable to transfer the request");

        return elle::Status::Ok;
      }

      ///
      /// this method stores a mutable block.
      ///
      elle::Status      Client::Put(const nucleus::Address&     address,
                                    const nucleus::MutableBlock& block)
      {
        //nucleus::Derivable<nucleus::Block>      derivable(address.component,
        //                                                  block);

        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::remote::Client::Put[Mutable]()\n");

        // check that the client is connected.
        if (this->socket == NULL)
          escape("the client seems to have been disconnected");

        // transfer to the remote.
        if (this->socket->Call(
              elle::network::Inputs<TagPush>(address, block),
              elle::network::Outputs<elle::TagOk>()) == elle::Status::Error)
          escape("unable to transfer the request");

        return elle::Status::Ok;
      }

      ///
      /// this method retrieves an immutable block.
      ///
      elle::Status      Client::Get(const nucleus::Address&     address,
                                    nucleus::ImmutableBlock&    block)
      {
        //nucleus::Derivable<nucleus::Block>      derivable(block);

        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::remote::Client::Get[Immutable]()\n");

        // check that the client is connected.
        if (this->socket == NULL)
          escape("the client seems to have been disconnected");

        // transfer to the remote.
        if (this->socket->Call(
              elle::network::Inputs<TagPull>(address,
                                    nucleus::Version::Any),
              elle::network::Outputs<TagBlock>(block)) == elle::Status::Error)
          escape("unable to transfer the request");

        return elle::Status::Ok;
      }

      ///
      /// this method retrieves a mutable block.
      ///
      elle::Status      Client::Get(const nucleus::Address&     address,
                                    const nucleus::Version&     version,
                                    nucleus::MutableBlock&      block)
      {
        //nucleus::Derivable<nucleus::Block>      derivable(block);

        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::remote::Client::Get[Mutable]()\n");

        // check that the client is connected.
        if (this->socket == NULL)
          escape("the client seems to have been disconnected");

        // transfer to the remote.
        if (this->socket->Call(
              elle::network::Inputs<TagPull>(address, version),
              elle::network::Outputs<TagBlock>(block)) == elle::Status::Error)
          escape("unable to transfer the request");

        return elle::Status::Ok;
      }

      ///
      /// this method removes a block.
      ///
      elle::Status      Client::Kill(const nucleus::Address&    address)
      {
        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::remote::Client::Kill()\n");

        // check that the client is connected.
        if (this->socket == NULL)
          escape("the client seems to have been disconnected");

        // transfer to the remote.
        if (this->socket->Call(
              elle::network::Inputs<TagWipe>(address),
              elle::network::Outputs<elle::TagOk>()) == elle::Status::Error)
          escape("unable to transfer the request");

        return elle::Status::Ok;
      }

//
// ---------- callbacks -------------------------------------------------------
//

      ///
      /// this callback is triggered whenever the socket is considered
      /// connected.
      ///
      elle::Status      Client::Connected()
      {
        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::remote::Client::Connected()\n");

        // set the client as connected.
        this->state = Client::StateConnected;

        return elle::Status::Ok;
      }

      ///
      /// this callback is triggered whenever the socket is considered
      /// disconnected.
      ///
      elle::Status      Client::Disconnected()
      {
        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::remote::Client::Disconnected()\n");

        // exit if the connection was shut down.
        if ((this->state == Client::StateConnected) ||
            (this->state == Client::StateAuthenticated))
          {
            // report the cause.
            report("the connection with the server has been shut down");

            // show the report.
            show();

            // exit the program.
            if (elle::concurrency::Program::Exit() == elle::Status::Error)
              escape("unable to exit the program");
          }

        return elle::Status::Ok;
      }

      ///
      /// this callback is triggered whenever an error occurs on the socket.
      ///
      elle::Status      Client::Error(elle::String)
      {
        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::remote::Client::Error()\n");

        // disconnect the socket, though that may be unecessary.

        // XXX: This this call is asynchronous (sigh), we might have
        // been destroyed, so check if the socket hasn't been
        // deleted. Don't touch anything else in this, also.
        if (this->socket)
          this->socket->Disconnect();

        return elle::Status::Ok;
      }

      ///
      /// this callback is triggered when the Authenticated message is
      /// received from the server meaning that the challenge has been passed.
      ///
      elle::Status      Client::Authenticated()
      {
        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::remote::Client::Authenticated()\n");

        // this client has succesfully been authenticated, set its state
        // accordingly.
        this->state = Client::StateAuthenticated;

        return elle::Status::Ok;
      }

      ///
      /// this callback is triggered whenever the Exception message is
      /// received which indicates the client that an error occured while
      /// processing one of its requests.
      ///
      elle::Status      Client::Exception(const elle::Report&   report)
      {
        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::remote::Client::Exception()\n");

        // transpose the given report.
        transpose(report);

        // log the error.
        log("an error occured on the server side");

        return elle::Status::Ok;
      }

//
// ---------- dumpable --------------------------------------------------------
//

      ///
      /// this method dumps the client.
      ///
      elle::Status      Client::Dump(const elle::Natural32      margin) const
      {
        elle::String    alignment(margin, ' ');

        std::cout << alignment << "[Client]" << std::endl;

        // dump the locus.
        if (this->locus.Dump(margin + 2) == elle::Status::Error)
          escape("unable to dump the locus");

        // dump the socket.
        if (this->socket != NULL)
          {
            if (this->socket->Dump(margin + 2) == elle::Status::Error)
              escape("unable to dump the socket");
          }
        else
          {
            std::cout << alignment << elle::Dumpable::Shift
                      << "[TCPSocket] " << elle::none << std::endl;
          }

        return elle::Status::Ok;
      }

    }
  }
}
