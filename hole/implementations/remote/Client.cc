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

ELLE_LOG_COMPONENT("Infinit.Hole.Remote.Client");

namespace hole
{
  namespace implementations
  {
    namespace remote
    {
      /*-------------.
      | Construction |
      `-------------*/

      Client::Client(std::string const& host, int port)
        : _state(State::connected)
        , _stream(elle::concurrency::scheduler(), host, port)
        , _serializer(elle::concurrency::scheduler(), _stream)
        , _channels(elle::concurrency::scheduler(), _serializer, true)
        , _rpc(_channels)
      {
        ELLE_LOG_TRACE("Authenticate to the server")
          if (_rpc.challenge(Hole::Passport))
            _state = State::authenticated;
      }

      Client::~Client()
      {}

      /*----.
      | API |
      `----*/

      void
      Client::put(const nucleus::proton::Address& address,
                  const nucleus::proton::ImmutableBlock& block)
      {
        nucleus::Derivable derivable(address.component, block,
                                     nucleus::Derivable::Kind::output);

        ELLE_LOG_TRACE("Put[Immutable]")
          this->_rpc.push(address, derivable);
      }

      void
      Client::put(const nucleus::proton::Address& address,
                  const nucleus::proton::MutableBlock& block)
      {
        nucleus::Derivable derivable(address.component, block);
        ELLE_LOG_TRACE("Put[Mutable]")
          this->_rpc.push(address, derivable);
      }


      std::unique_ptr<nucleus::proton::Block>
      Client::get(const nucleus::proton::Address& address)
      {
        ELLE_LOG_TRACE("Get[Immutable]")
          return this->_rpc.pull(address, nucleus::proton::Version::Any).release();
      }

      std::unique_ptr<nucleus::proton::Block>
      Client::get(const nucleus::proton::Address& address,
                  const nucleus::proton::Version& version)
      {
        ELLE_LOG_TRACE("Get[Mutable]")
          return this->_rpc.pull(address, version).release();
      }

      void
      Client::kill(const nucleus::proton::Address& address)
      {
        ELLE_LOG_TRACE("Kill")
          this->_rpc.wipe(address);
      }

      /*---------.
      | Dumpable |
      `---------*/

      elle::Status
      Client::Dump(const elle::Natural32 margin) const
      {
        elle::String alignment(margin, ' ');
        std::cout << alignment << "[Client]" << std::endl;
        return elle::Status::Ok;
      }

    }
  }
}
