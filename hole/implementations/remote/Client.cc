#include <elle/log.hh>

#include <elle/standalone/Report.hh>
#include <elle/network/Bundle.hh>
#include <elle/utility/Buffer.hh>
#include <elle/network/Header.hh>

#include <elle/concurrency/Program.hh>
#include <elle/network/Locus.hh>
#include <elle/network/Range.hh>
#include <elle/network/TCPSocket.hh>
#include <elle/utility/Time.hh>
#include <elle/network/Network.hh>

#include <lune/Passport.hh>
#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Version.hh>
#include <nucleus/proton/Block.hh>

#include <hole/implementations/remote/Client.hh>
#include <hole/implementations/remote/Manifest.hh>

#include <hole/Hole.hh>

#include <Infinit.hh>

ELLE_LOG_TRACE_COMPONENT("Infinit.Hole.Remote.Client");

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
        ELLE_LOG_TRACE_SCOPE("Launch");

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
        }

        {
          std::string hostname;
          this->locus.host.Convert(hostname);
          auto socket = new reactor::network::TCPSocket
            (elle::concurrency::scheduler(), hostname, this->locus.port);
          this->socket = new elle::network::TCPSocket(socket);
          Connected();
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

        ELLE_LOG_TRACE_SCOPE("Put[Immutable]");

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

        ELLE_LOG_TRACE_SCOPE("Put[Mutable]");

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

        ELLE_LOG_TRACE_SCOPE("Get[Immutable]");

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

        ELLE_LOG_TRACE_SCOPE("Get[Mutable]");

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
        ELLE_LOG_TRACE_SCOPE("Kill");

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
        ELLE_LOG_TRACE_SCOPE("Connected");

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
        ELLE_LOG_TRACE_SCOPE("Disconnected");

        // exit if the connection was shut down.
        if ((this->state == Client::StateConnected) ||
            (this->state == Client::StateAuthenticated))
          {
            // report the cause.
            report("the connection with the server has been shut down");

            // show the report.
            show();

            elle::Program::Exit();
          }

        return elle::Status::Ok;
      }

      ///
      /// this callback is triggered whenever an error occurs on the socket.
      ///
      elle::Status      Client::Error(elle::String)
      {
        ELLE_LOG_TRACE_SCOPE("Error");

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
      elle::Status
      Client::Authenticated()
      {
        ELLE_LOG_TRACE_SCOPE("Authenticated");

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
        ELLE_LOG_TRACE_SCOPE("Exception");

        // transpose the given report.
        transpose(report);

        // log the error.
        log("an error occured on the server side");

        return elle::Status::Ok;
      }


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

        return elle::Status::Ok;
      }

    }
  }
}
