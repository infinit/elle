#include <Infinit.hh>

#include <elle/standalone/Morgue.hh>
#include <elle/network/TCPSocket.hh>

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

      elle::concurrency::scheduler().CallLater
        (boost::bind(&Application::Abort, this),
         "Application abort", Application::Timeout);

      return elle::Status::Ok;
    }

//
// ---------- callbacks -------------------------------------------------------
//

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
      std::cout << alignment << elle::io::Dumpable::Shift
                << "[State] " << std::dec << this->state << std::endl;

      // dump the processing.
      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Processing] " << std::dec
                << this->processing << std::endl;

      return elle::Status::Ok;
    }

  }
}
