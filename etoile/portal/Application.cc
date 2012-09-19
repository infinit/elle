#include <Infinit.hh>

#include <elle/serialize/PairSerializer.hxx>

#include <reactor/network/tcp-socket.hh>

#include <protocol/Serializer.hh>

#include <etoile/abstract/Group.hh>
#include <etoile/portal/Application.hh>
#include <etoile/portal/Portal.hh>
#include <etoile/wall/Access.hh>
#include <etoile/wall/Group.hh>
#include <etoile/wall/Object.hh>
#include <etoile/wall/Path.hh>

ELLE_LOG_COMPONENT("infinit.etoile.portal.Application");

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
    Application::Application():
      state(StateDisconnected),
      processing(ProcessingOff),
      socket(nullptr)
    {}

    ///
    /// destructor.
    ///
    Application::~Application()
    {
      Portal::applications.erase(this->socket);
      if (this->socket != nullptr)
        delete this->socket;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates the application given its socket.
    ///
    elle::Status        Application::Create(reactor::network::TCPSocket*  socket)
    {
      // set the socket.
      this->socket = socket;
      this->serializer =
        new infinit::protocol::Serializer(elle::concurrency::scheduler(), *this->socket);
      this->channels =
        new infinit::protocol::ChanneledStream(elle::concurrency::scheduler(),
                                               *this->serializer, false);

      // Setup RPCs.
      this->rpcs = new etoile::portal::RPC(*this->channels);

      this->rpcs->accessconsult = wall::Access::consult;
      this->rpcs->accessgrant = &wall::Access::Grant;
      this->rpcs->accesslookup = &wall::Access::lookup;
      this->rpcs->accessrevoke = &wall::Access::Revoke;
      this->rpcs->authenticate = boost::bind(&Application::_authenticate, this, _1);
      this->rpcs->groupadd = &wall::Group::Add;
      this->rpcs->groupconsult = &wall::Group::Consult;
      this->rpcs->groupcreate = &wall::Group::Create;
      this->rpcs->groupdestroy = &wall::Group::Destroy;
      this->rpcs->groupdiscard = &wall::Group::Discard;
      this->rpcs->groupinformation = &wall::Group::Information;
      this->rpcs->groupload = &wall::Group::Load;
      this->rpcs->grouplookup = &wall::Group::Lookup;
      this->rpcs->groupremove = &wall::Group::Remove;
      this->rpcs->groupstore = &wall::Group::Store;
      this->rpcs->objectdiscard = &wall::Object::Discard;
      this->rpcs->objectload = &wall::Object::Load;
      this->rpcs->objectstore = &wall::Object::Store;
      this->rpcs->pathresolve = &wall::Path::resolve;

      new reactor::Thread(elle::concurrency::scheduler(),
                          elle::sprintf("RPC %s", *this),
                          std::bind(&Application::_run, this), true);

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

    /*----------.
    | Printable |
    `----------*/

    void
    Application::print(std::ostream& stream) const
    {
      stream << "App " << socket->peer();
    }

    /*-------------.
    | RPC handlers |
    `-------------*/

    void
    Application::_run()
    {
      try
        {
          this->rpcs->run();
        }
      catch (std::runtime_error& e)
        {
          ELLE_ERR("%s: fatal RPC error: %s", *this, e.what());
        }
      catch (...)
        {
          ELLE_ERR("%s: unknown fatal RPC errer", *this);
        }
      delete this;
    }

    class Ward
    {
    public:
      // Check the application has been authenticated.
      Ward(Application* app)
      : _app(app)
      {
        if (_app->state != Application::StateAuthenticated)
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "the application has not been authenticated");
        _app->processing = Application::ProcessingOn;
      }

      ~Ward()
      {
        // FIXME: if several call happen in parallel, this won't do it.
        _app->processing = Application::ProcessingOff;
        // Remove the application's if it has been disconnected.
        if (_app->state == Application::StateDisconnected)
          {
            Portal::Remove(_app->socket);
            delete _app;
          }
      }

    private:
      Application* _app;
    };

    bool
    Application::_authenticate(std::string const& pass)
    {
      ELLE_TRACE_SCOPE("Authenticate()");

      if (Portal::phrase.pass != pass)
        return false;
      this->state = Application::StateAuthenticated;
      return true;
    }
  }
}
