#include <elle/utility/Time.hh>
#include <elle/standalone/Morgue.hh>

#include <hole/implementations/slug/Host.hh>
#include <hole/implementations/slug/Slug.hh>
#include <hole/implementations/slug/Manifest.hh>
#include <hole/Hole.hh>

#include <Infinit.hh>

namespace hole
{
  namespace implementations
  {
    namespace slug
    {

//
// ---------- definitions -----------------------------------------------------
//

      ///
      /// XXX 2 sec
      ///
      const elle::Natural32             Host::Timeout = 1000;

//
// ---------- constructors & destructors --------------------------------------
//

      ///
      /// XXX
      ///
      Host::Host(const elle::network::Locus& locus):
        state(StateUnknown),
        locus(locus),
        socket(nullptr)
      {
        std::string hostname;
        if (this->locus.host.Convert(hostname) == elle::Status::Error)
          throw std::runtime_error("unable to convert the hostname into a "
                                   "valid locus");
        auto socket =
          new reactor::network::TCPSocket(elle::concurrency::scheduler(),
                                          hostname,
                                          this->locus.port);
        this->socket = new elle::network::TCPSocket(socket);
        Connected();
      }

      ///
      /// XXX
      ///
      Host::Host(elle::network::TCPSocket* connection)
      {
        this->socket = connection;
        this->state = Host::StateConnected;
      }

      ///
      /// XXX
      ///
      Host::~Host()
      {
        // delete the socket, if present.
        if (this->socket != NULL)
          delete this->socket;
      }

//
// ---------- methods ---------------------------------------------------------

      ///
      /// XXX
      ///
      elle::Status      Host::Disconnect()
      {
        this->socket->Disconnect();

        return elle::Status::Ok;
      }

      ///
      /// XXX
      ///
      elle::Status      Host::Authenticated()
      {
        this->state = Host::StateAuthenticated;

        return elle::Status::Ok;
      }

//
// ---------- callbacks -------------------------------------------------------
//

      ///
      /// XXX
      ///
      elle::Status      Host::Abort()
      {
        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::slug::Host::Abort()\n");

        // if the host has not been authenticated...
        if (this->state != Host::StateAuthenticated)
          {
            // emit the signal.
            if (this->signal.dead.Emit(this) == elle::Status::Error)
              escape("unable to emit the signal");

            // set the state.
            this->state = Host::StateDead;
          }

        return elle::Status::Ok;
      }

      ///
      /// XXX
      ///
      elle::Status      Host::Connected()
      {
        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::slug::Host::Connected()\n");

        // set the state.
        this->state = Host::StateConnected;

        return elle::Status::Ok;
      }

      ///
      /// XXX
      ///
      elle::Status      Host::Disconnected()
      {
        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::slug::Host::Disconnected()\n");

        // emit the signal.
        if (this->signal.dead.Emit(this) == elle::Status::Error)
          escape("unable to emit the signal");

        // set the state.
        this->state = Host::StateDead;

        return elle::Status::Ok;
      }

      ///
      /// XXX
      ///
      elle::Status      Host::Error(elle::String)
      {
        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::slug::Host::Error()\n");

        //
        // nothing to do.
        //

        return elle::Status::Ok;
      }

//
// ---------- dumpable --------------------------------------------------------
//

      ///
      /// this function dumps a host object.
      ///
      elle::Status      Host::Dump(elle::Natural32      margin) const
      {
        elle::String    alignment(margin, ' ');

        // display the name.
        std::cout << alignment << "[Host] "
                  << std::hex << this << std::endl;

        // display the state.
        std::cout << alignment << elle::Dumpable::Shift
                  << "[State] " << std::dec << this->state << std::endl;

        // dump the locus.
        if (this->locus.Dump(margin + 2) == elle::Status::Error)
          escape("unable to dump the locus");

        return elle::Status::Ok;
      }

    }
  }
}
