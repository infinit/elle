#include <Infinit.hh>
#include <elle/standalone/Morgue.hh>

#include <etoile/portal/Application.hh>
#include <etoile/portal/Portal.hh>

namespace etoile
{
  namespace portal
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this constant defines the time after which an application is
    /// rejected if it has not been authenticated.
    ///
    const reactor::Duration Application::Timeout =
      boost::posix_time::seconds(5);

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Application::Application()
      : state(StateDisconnected)
      , processing(ProcessingOff)
      , socket(NULL)
    {}

    ///
    /// destructor.
    ///
    Application::~Application()
    {
      // delete the socket, if present.
      if (this->socket != NULL)
        delete this->socket;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates the application given its socket.
    ///
    elle::Status        Application::Create(elle::network::TCPSocket*  socket)
    {
      // set the socket.
      this->socket = socket;

      // set the state.
      this->state = Application::StateConnected;

      // // subscribe to the signal.
      // if (this->socket->signal.error.Subscribe(
      //       elle::Callback<>::Infer(&Application::Error,
      //                               this)) == elle::Status::Error)
      //   escape("unable to subscribe to the signal");

      elle::concurrency::scheduler().CallLater
        (boost::bind(&Application::Abort, this),
         "Application abort", Application::Timeout);

      return elle::Status::Ok;
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this callback is triggered whenever the application is considered
    /// disconnected.
    ///
    elle::Status        Application::Disconnected()
    {
      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        std::cout << "[etoile] portal::Application::Disconnected()"
                  << std::endl;

      // set the application's state as disconnected.
      this->state = Application::StateDisconnected;

      // check if the application is currently processing.
      if (this->processing == Application::ProcessingOff)
        {
          // remove the application from the portal.
          if (Portal::Remove(this->socket) == elle::Status::Error)
            escape("unable to remove the application from the portal");

          // bury the application.
          bury(this);
        }

      return elle::Status::Ok;
    }

    ///
    /// this callback is triggered whenever an error occurs on the
    /// application's socket.
    ///
    elle::Status        Application::Error(elle::String)
    {
      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        std::cout << "[etoile] portal::Application::Error()"
                  << std::endl;

      // disconnect the socket, though that may be unecessary i.e do not
      // check the return status.
      this->socket->Disconnect();

      return elle::Status::Ok;
    }

    ///
    /// this callback is triggered once the authentication timer times out.
    ///
    /// if the application has not been authenticated, it is destroyed.
    ///
    elle::Status        Application::Abort()
    {
      // Check if the application has been authenticated.
      if (this->state == Application::StateAuthenticated)
        return elle::Status::Ok;
      if (Infinit::Configuration.etoile.debug == true)
        std::cout << "[etoile] portal::Application::Abort()" << std::endl;
      return elle::Status::Ok;
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an application.
    ///
    elle::Status        Application::Dump(elle::Natural32       margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Application]" << std::endl;

      // dump the state.
      std::cout << alignment << elle::Dumpable::Shift
                << "[State] " << std::dec << this->state << std::endl;

      // dump the processing.
      std::cout << alignment << elle::Dumpable::Shift
                << "[Processing] " << std::dec
                << this->processing << std::endl;

      return elle::Status::Ok;
    }

  }
}
