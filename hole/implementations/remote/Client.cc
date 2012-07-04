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

      /*-------------.
      | Construction |
      `-------------*/

      Client::Client(std::string const& host, int port)
        : state(Client::StateUnknown)
        , _stream(elle::concurrency::scheduler(), host, port)
        , _serializer(elle::concurrency::scheduler(), _stream)
        , _channels(elle::concurrency::scheduler(), _serializer, true)
        , _rpc(_channels)
      {
        this->state = Client::StateConnected;

        // Authenticate to the server.
        ELLE_LOG_TRACE("Authenticate to the server")
          _rpc.challenge(Hole::Passport);
      }

      Client::~Client()
      {}

//
// ---------- peer ------------------------------------------------------------
//

      ///
      /// this method launches the client.
      ///
      elle::Status      Client::Launch()
      {
        ELLE_LOG_TRACE_SCOPE("Launch");

        return elle::Status::Ok;
      }

      void
      Client::Put(const nucleus::proton::Address& address,
                  const nucleus::proton::ImmutableBlock& block)
      {
        nucleus::Derivable derivable(
          address.component,
          block,
          nucleus::Derivable::Kind::output);

        ELLE_LOG_TRACE("Put[Immutable]")
          this->_rpc.push(address, derivable);
      }

      void
      Client::Put(const nucleus::proton::Address& address,
                  const nucleus::proton::MutableBlock& block)
      {
        nucleus::Derivable derivable(address.component, block);

        ELLE_LOG_TRACE("Put[Mutable]")
          this->_rpc.push(address, derivable);
      }


      std::unique_ptr<nucleus::proton::Block>
      Client::Get(const nucleus::proton::Address& address)
      {
        ELLE_LOG_TRACE("Get[Immutable]")
            return this->_rpc.pull(address, nucleus::proton::Version::Any).release();
      }

      std::unique_ptr<nucleus::proton::Block>
      Client::Get(const nucleus::proton::Address& address,
                  const nucleus::proton::Version& version)
      {
        ELLE_LOG_TRACE("Get[Mutable]")
          return this->_rpc.pull(address, version).release();
      }

      void
      Client::Kill(const nucleus::proton::Address& address)
      {
        ELLE_LOG_TRACE("Kill")
          this->_rpc.wipe(address);
      }

//
// ---------- callbacks -------------------------------------------------------
//

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

        return elle::Status::Ok;
      }

    }
  }
}
