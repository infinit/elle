#include <elle/log.hh>

#include <hole/Hole.hh>
#include <hole/implementations/remote/Client.hh>
#include <hole/implementations/remote/Manifest.hh>

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
      Client::Client(const elle::Locus&                         locus):
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
          if (elle::Network::Register(
                elle::Procedure<TagAuthenticated>(
                  elle::Callback<>::Infer(
                    &Client::Authenticated, this))) == elle::StatusError)
            escape("unable to register the callback");

          // register the message.
          if (elle::Network::Register(
                elle::Procedure<TagException>(
                  elle::Callback<>::Infer(
                    &Client::Exception, this))) == elle::StatusError)
            escape("unable to register the callback");
        }

        {
          std::string hostname;
          this->locus.host.Convert(hostname);
          auto socket = new reactor::network::TCPSocket
            (elle::concurrency::scheduler(), hostname, this->locus.port);
          this->socket = new elle::TCPSocket(socket);
          Connected();
        }

        //
        // authenticate to the server.
        //
        {
          // send the passport.
          if (this->socket->Send(
                elle::Inputs<TagChallenge>(Hole::Passport)) ==
              elle::StatusError)
            escape("unable to send the challenge");
        }

        return elle::StatusOk;
      }

      ///
      /// this method stores an immutable block.
      ///
      elle::Status      Client::Put(const nucleus::Address&     address,
                                    const nucleus::ImmutableBlock& block)
      {
        nucleus::Derivable<nucleus::Block>      derivable(address.component,
                                                          block);

        ELLE_LOG_TRACE_SCOPE("Put[Immutable]");

        // check that the client is connected.
        if (this->socket == NULL)
          escape("the client seems to have been disconnected");

        // transfer to the remote.
        if (this->socket->Call(
              elle::Inputs<TagPush>(address, derivable),
              elle::Outputs<elle::TagOk>()) == elle::StatusError)
          escape("unable to transfer the request");

        return elle::StatusOk;
      }

      ///
      /// this method stores a mutable block.
      ///
      elle::Status      Client::Put(const nucleus::Address&     address,
                                    const nucleus::MutableBlock& block)
      {
        nucleus::Derivable<nucleus::Block>      derivable(address.component,
                                                          block);

        ELLE_LOG_TRACE_SCOPE("Put[Mutable]");

        // check that the client is connected.
        if (this->socket == NULL)
          escape("the client seems to have been disconnected");

        // transfer to the remote.
        if (this->socket->Call(
              elle::Inputs<TagPush>(address,
                                    derivable),
              elle::Outputs<elle::TagOk>()) == elle::StatusError)
          escape("unable to transfer the request");

        return elle::StatusOk;
      }

      ///
      /// this method retrieves an immutable block.
      ///
      elle::Status      Client::Get(const nucleus::Address&     address,
                                    nucleus::ImmutableBlock&    block)
      {
        nucleus::Derivable<nucleus::Block>      derivable(block);

        ELLE_LOG_TRACE_SCOPE("Get[Immutable]");

        // check that the client is connected.
        if (this->socket == NULL)
          escape("the client seems to have been disconnected");

        // transfer to the remote.
        if (this->socket->Call(
              elle::Inputs<TagPull>(address,
                                    nucleus::Version::Any),
              elle::Outputs<TagBlock>(derivable)) == elle::StatusError)
          escape("unable to transfer the request");

        return elle::StatusOk;
      }

      ///
      /// this method retrieves a mutable block.
      ///
      elle::Status      Client::Get(const nucleus::Address&     address,
                                    const nucleus::Version&     version,
                                    nucleus::MutableBlock&      block)
      {
        nucleus::Derivable<nucleus::Block>      derivable(block);

        ELLE_LOG_TRACE_SCOPE("Get[Mutable]");

        // check that the client is connected.
        if (this->socket == NULL)
          escape("the client seems to have been disconnected");

        // transfer to the remote.
        if (this->socket->Call(
              elle::Inputs<TagPull>(address,
                                    version),
              elle::Outputs<TagBlock>(derivable)) == elle::StatusError)
          escape("unable to transfer the request");

        return elle::StatusOk;
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
              elle::Inputs<TagWipe>(address),
              elle::Outputs<elle::TagOk>()) == elle::StatusError)
          escape("unable to transfer the request");

        return elle::StatusOk;
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

        return elle::StatusOk;
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

        return elle::StatusOk;
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

        return elle::StatusOk;
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

        return elle::StatusOk;
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

        return elle::StatusOk;
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
        if (this->locus.Dump(margin + 2) == elle::StatusError)
          escape("unable to dump the locus");

        return elle::StatusOk;
      }

    }
  }
}
