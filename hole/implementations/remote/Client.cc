#include <hole/implementations/remote/Client.hh>
#include <hole/implementations/remote/Manifest.hh>

#include <elle/log.hh>
#include <elle/standalone/Report.hh>
#include <elle/network/Locus.hh>
#include <elle/network/TCPSocket.hh>
#include <elle/network/Network.hh>
#include <elle/network/Inputs.hh>
#include <elle/network/Outputs.hh>

#include <lune/Passport.hh>

#include <nucleus/Derivable.hh>
#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Version.hh>
#include <nucleus/proton/ImmutableBlock.hh>
#include <nucleus/proton/MutableBlock.hh>

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
      Client::Client(const elle::network::Locus& locus):
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
          this->socket = new elle::network::TCPSocket(socket, true);
          this->state = Client::StateConnected;
        }

        // Authenticate to the server.
        ELLE_LOG_TRACE("Authenticate to the server")
          this->socket->send(elle::network::Inputs<TagChallenge>(Hole::Passport));

        return elle::Status::Ok;
      }

      ///
      /// this method stores an immutable block.
      ///
      elle::Status      Client::Put(const nucleus::proton::Address& address,
                                    const nucleus::proton::ImmutableBlock& block)
      {
        nucleus::Derivable derivable(
          address.component,
          block,
          nucleus::Derivable::Kind::output);

        ELLE_LOG_TRACE_SCOPE("Put[Immutable]");

        // check that the client is connected.
        if (this->socket == NULL)
          escape("the client seems to have been disconnected");

        // transfer to the remote.
        if (this->socket->Call(
              elle::network::Inputs<TagPush>(address, derivable),
              elle::network::Outputs<elle::TagOk>()) == elle::Status::Error)
          escape("unable to transfer the request");

        return elle::Status::Ok;
      }

      ///
      /// this method stores a mutable block.
      ///
      elle::Status      Client::Put(const nucleus::proton::Address& address,
                                    const nucleus::proton::MutableBlock& block)
      {
        nucleus::Derivable derivable(address.component, block);

        ELLE_LOG_TRACE_SCOPE("Put[Mutable]");

        // check that the client is connected.
        if (this->socket == NULL)
          escape("the client seems to have been disconnected");

        // transfer to the remote.
        if (this->socket->Call(
              elle::network::Inputs<TagPush>(address, derivable),
              elle::network::Outputs<elle::TagOk>()) == elle::Status::Error)
          escape("unable to transfer the request");

        return elle::Status::Ok;
      }

      ///
      /// this method retrieves an immutable block.
      ///
      elle::Status      Client::Get(const nucleus::proton::Address& address,
                                    nucleus::proton::ImmutableBlock&  block)
      {
        nucleus::Derivable derivable(
          address.component,
          block,
          nucleus::Derivable::Kind::output);

        ELLE_LOG_TRACE_SCOPE("Get[Immutable]");

        // check that the client is connected.
        if (this->socket == NULL)
          escape("the client seems to have been disconnected");

        // transfer to the remote.
        if (this->socket->Call(
              elle::network::Inputs<TagPull>(address,
                                    nucleus::proton::Version::Any),
              elle::network::Outputs<TagBlock>(derivable)) == elle::Status::Error)
          escape("unable to transfer the request");

        return elle::Status::Ok;
      }

      ///
      /// this method retrieves a mutable block.
      ///
      elle::Status      Client::Get(const nucleus::proton::Address& address,
                                    const nucleus::proton::Version& version,
                                    nucleus::proton::MutableBlock& block)
      {
        nucleus::Derivable derivable(
          address.component,
          block,
          nucleus::Derivable::Kind::output);

        ELLE_LOG_TRACE_SCOPE("Get[Mutable]");

        // check that the client is connected.
        if (this->socket == NULL)
          escape("the client seems to have been disconnected");

        // transfer to the remote.
        if (this->socket->Call(
              elle::network::Inputs<TagPull>(address, version),
              elle::network::Outputs<TagBlock>(derivable)) == elle::Status::Error)
          escape("unable to transfer the request");

        return elle::Status::Ok;
      }

      ///
      /// this method removes a block.
      ///
      elle::Status      Client::Kill(const nucleus::proton::Address& address)
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
      elle::Status      Client::Exception(
        const elle::standalone::Report& report)
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
