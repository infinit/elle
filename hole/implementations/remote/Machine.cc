#include <hole/implementations/remote/Implementation.hh>
#include <hole/implementations/remote/Machine.hh>
#include <hole/implementations/remote/Client.hh>
#include <hole/implementations/remote/Server.hh>
#include <hole/Hole.hh>

#include <reactor/network/exception.hh>

#include <elle/log.hh>

#include <elle/idiom/Close.hh>
# include <boost/format.hpp>
#include <elle/idiom/Open.hh>

ELLE_LOG_COMPONENT("Infinit.Hole.Remote.Machine");

namespace hole
{
  namespace implementations
  {
    namespace remote
    {

      /*-------------.
      | Construction |
      `-------------*/

      Machine::Machine(Implementation& hole):
        _hole(hole),
        role(Machine::RoleUnknown)
      {}

      ///
      /// destructor.
      ///
      Machine::~Machine()
      {
        // delete the entity depending on the role.
        switch (this->role)
          {
          case Machine::RoleServer:
            {
              delete this->server;

              break;
            }
          case Machine::RoleClient:
            {
              delete this->client;

              break;
            }
          case Machine::RoleUnknown:
            {
              // nothing to do.

              break;
            }
          }
      }

//
// ---------- methods ---------------------------------------------------------
//

      ///
      /// this method launches the machine which tries to act first as a
      /// client and then as a server if no server seems to exist.
      ///
      void
      Machine::Launch()
      {
        // try to connect to the server's host.
        ELLE_TRACE("try starting as a client")
          try
            {
              std::string host;
              _hole.server_locus().host.Convert(host);
              auto client = std::unique_ptr<Client>(
                new Client(this->_hole.passport(), host,
                           _hole.server_locus().port));
              this->role = Machine::RoleClient;
              this->_hole.ready();
              this->client = client.release();
              return;
            }
          catch (reactor::network::Exception& e)
            {
              ELLE_TRACE("network error while starting as a client: %s", e.what());
            }

        // purge the error messages.
        purge();

        // if the client did not succeed, create a server a wait for a client.
        ELLE_TRACE("start as a server")
        {
          this->server = new Server(_hole, _hole.server_locus().port);
          this->role = Machine::RoleServer;
          return;
        }

        throw std::runtime_error("unable to create a client or a server");
      }

      /*---------.
      | Dumpable |
      `---------*/
      elle::Status
      Machine::Dump(const elle::Natural32 margin) const
      {
        elle::String alignment(margin, ' ');

        std::cout << alignment << "[Machine]" << std::endl;

        // dump the role.
        std::cout << alignment << elle::io::Dumpable::Shift
                  << "[Role] " << this->role << std::endl;

        // dump the entity.
        switch (this->role)
          {
          case Machine::RoleServer:
            {
              // dump the server.
              if (this->server->Dump(margin + 2) == elle::Status::Error)
                escape("unable to dump the server");

              break;
            }
          case Machine::RoleClient:
            {
              // dump the client.
              if (this->client->Dump(margin + 2) == elle::Status::Error)
                escape("unable to dump the client");

              break;
            }
          default:
            {
              // nothing to do.

              break;
            }
          }

        return elle::Status::Ok;
      }

    }
  }
}
