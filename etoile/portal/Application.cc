//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [tue nov  1 08:51:06 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/portal/Application.hh>
#include <etoile/portal/Portal.hh>

#include <Infinit.hh>

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
    const elle::Natural32               Application::Timeout = 5000;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Application::Application():
      state(StateDisconnected),
      processing(ProcessingOff),
      timer(NULL),
      socket(NULL)
    {
    }

    ///
    /// destructor.
    ///
    Application::~Application()
    {
      // delete the timer, if present.
      if (this->timer != NULL)
        delete this->timer;

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
    elle::Status        Application::Create(elle::LocalSocket*  socket)
    {
      // set the socket.
      this->socket = socket;

      // set the state.
      this->state = Application::StateConnected;

      // subscribe to the signal.
      if (this->socket->signal.disconnected.Subscribe(
            elle::Callback<>::Infer(&Application::Disconnected,
                                    this)) == elle::Status::Error)
        escape("unable to subscribe to the signal");

      // subscribe to the signal.
      if (this->socket->signal.error.Subscribe(
            elle::Callback<>::Infer(&Application::Error,
                                    this)) == elle::Status::Error)
        escape("unable to subscribe to the signal");

      // allocate a new timer.
      this->timer = new elle::concurrency::Timer;

      // create the timer.
      if (this->timer->Create(elle::concurrency::Timer::ModeSingle) == elle::Status::Error)
        escape("unable to create the timer");

      // subscribe to the timer's signal.
      if (this->timer->signal.timeout.Subscribe(
            elle::Callback<>::Infer(&Application::Abort,
                                    this)) == elle::Status::Error)
        escape("unable to subscribe to the signal");

      // start the timer.
      if (this->timer->Start(Application::Timeout) == elle::Status::Error)
        escape("unable to start the timer");

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
    elle::Status        Application::Error(const elle::String&)
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
      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        std::cout << "[etoile] portal::Application::Abort()"
                  << std::endl;

      // bury the timer because we are in the timer to delete.
      bury(this->timer);

      // reset the pointer.
      this->timer = NULL;

      // check if the application has been authenticated.
      if (this->state != Application::StateAuthenticated)
        {
          // disconnect the application.
          if (this->socket->Disconnect() == elle::Status::Error)
            escape("unable to disconnect the socket");
        }

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

      // dump the timer, depending on its presence.
      if (this->timer != NULL)
        {
          // dump the timer.
          if (this->timer->Dump(margin + 2) == elle::Status::Error)
            escape("unable to dump the timer");
        }
      else
        {
          // dump a null timer.
          std::cout << alignment << elle::Dumpable::Shift
                    << "[Timer] " << elle::none << std::endl;
        }

      // dump the socket, depending on its presence.
      if (this->socket != NULL)
        {
          // dump the socket.
          if (this->socket->Dump(margin + 2) == elle::Status::Error)
            escape("unable to dump the socket");
        }
      else
        {
          // dump a null socket.
          std::cout << alignment << elle::Dumpable::Shift
                    << "[LocalSocket] " << elle::none << std::endl;
        }

      return elle::Status::Ok;
    }

  }
}
