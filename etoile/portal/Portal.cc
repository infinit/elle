#include <elle/cryptography/Random.hh>
#include <elle/io/Path.hh>
#include <elle/io/Piece.hh>
#include <elle/log.hh>
#include <elle/serialize/PairSerializer.hxx>
#include <elle/standalone/Morgue.hh>
#include <elle/standalone/Report.hh>

#include <reactor/network/tcp-server.hh>
#include <reactor/thread.hh>

#include <nucleus/neutron/Trait.hh>
#include <nucleus/neutron/Range.hh>
#include <nucleus/neutron/Record.hh>
#include <nucleus/neutron/Entry.hh>
#include <nucleus/neutron/Fellow.hh>

#include <etoile/portal/Portal.hh>
#include <etoile/path/Way.hh>
#include <etoile/path/Chemin.hh>
#include <etoile/path/Path.hh>
#include <etoile/gear/Identifier.hh>
#include <etoile/abstract/Object.hh>
#include <etoile/abstract/Group.hh>
#include <etoile/portal/Manifest.hh>

#include <etoile/wall/Object.hh>
#include <etoile/wall/File.hh>
#include <etoile/wall/Directory.hh>
#include <etoile/wall/Link.hh>
#include <etoile/wall/Access.hh>
#include <etoile/wall/Attributes.hh>
#include <etoile/wall/Path.hh>
#include <etoile/wall/Group.hh>

#include <lune/Lune.hh>

#include <Infinit.hh>

#include <elle/idiom/Open.hh>

ELLE_LOG_COMPONENT("infinit.etoile.portal.Portal");

namespace etoile
{
  namespace portal
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// the container holding the connected applications.
    ///
    Portal::Container Portal::applications;

    reactor::network::TCPServer* Portal::server = nullptr;
    reactor::Thread* Portal::acceptor = nullptr;

    ///
    /// this variable contains the phrase enabling applications to
    /// request operations on Etoile.
    ///
    lune::Phrase Portal::phrase;

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the portal system.
    ///
    elle::Status        Portal::Initialize()
    {
      elle::network::Port port;

      ELLE_TRACE("register the messages");

      try
        {
          ELLE_TRACE("set up the server and allocator");

          // allocate the server and acceptor for handling incoming connections.
          Portal::server =
            new reactor::network::TCPServer(elle::concurrency::scheduler());
          Portal::acceptor =
            new reactor::Thread(elle::concurrency::scheduler(),
                                "Portal Server accept",
                                boost::bind(&Portal::accept));

          // finally, listen for incoming connections.
          Portal::server->listen();
          port = Portal::server->local_endpoint().port();

          ELLE_TRACE("listening on %u", port);
        }
      catch (std::runtime_error& e)
        {
          escape("unable to set up the portal server: %s",
                 e.what());
        }

      // generate a phrase randomly which will be used by applications to
      // connect to Etoile and trigger specific actions.
      //
      // generate a random string, create a phrase with it along with
      // the socket used by portal so that applications have everything
      // to connect to and authenticate to portal.
      elle::String pass;

      if (elle::cryptography::Random::Generate(pass) == elle::Status::Error)
        escape("unable to generate a random string");

      if (Portal::phrase.Create(port,
                                pass) == elle::Status::Error)
        escape("unable to create the phrase");

      Portal::phrase.store(Infinit::Network, "portal");

      return elle::Status::Ok;
    }

    ///
    /// this method cleans the portal system.
    ///
    elle::Status        Portal::Clean()
    {
      Portal::Scoutor scoutor;

      lune::Phrase::erase(Infinit::Network, "portal");

      // delete the acceptor.
      Portal::acceptor->terminate_now();
      delete Portal::acceptor;
      Portal::acceptor = nullptr;

      // delete the applications.
      for (scoutor = Portal::applications.begin();
           scoutor != Portal::applications.end();
           scoutor++)
        {
          Application* application = scoutor->second;
          delete application;
        }

      Portal::applications.clear();

      // delete the server.
      delete Portal::server;
      Portal::server = nullptr;

      return elle::Status::Ok;
    }

    ///
    /// this method registers an application in the set of connected
    /// applications.
    ///
    elle::Status        Portal::Add(Application*                application)
    {
      std::pair<Portal::Iterator, elle::Boolean>        result;

      // check if this application has already been registered.
      if (Portal::applications.find(application->socket) !=
          Portal::applications.end())
        escape("this application seems to have already been registered");

      // insert the application in the container.
      result =
        Portal::applications.insert(
          Portal::Value(application->socket, application));

      // check if the insertion was successful.
      if (result.second == false)
        escape("unable to insert the application in the container");

      return elle::Status::Ok;
    }

    ///
    /// this method removes an application from the container.
    ///
    elle::Status        Portal::Remove(reactor::network::TCPSocket*       socket)
    {
      Portal::Iterator  iterator;

      // locate the entry.
      if ((iterator =
           Portal::applications.find(socket)) == Portal::applications.end())
        escape("unable to locate the given socket");

      // erase the entry.
      Portal::applications.erase(iterator);

      return elle::Status::Ok;
    }

    ///
    /// this method dumps the whole portal.
    ///
    elle::Status        Portal::Show(const elle::Natural32      margin)
    {
      elle::String      alignment(margin, ' ');
      Portal::Scoutor   scoutor;

      std::cout << alignment << "[Portal]" << std::endl;

      // dump the applications.
      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Applications]" << std::endl;

      // go through the applications.
      for (scoutor = Portal::applications.begin();
           scoutor != Portal::applications.end();
           scoutor++)
        {
          Application*  application = scoutor->second;

          // dump the application.
          if (application->Dump(margin + 4) == elle::Status::Error)
            escape("unable to dump the application");
        }

      return elle::Status::Ok;
    }

//
// ---------- callbacks -------------------------------------------------------
//

    void
    Portal::accept()
    {
      while (true)
        {
          reactor::network::TCPSocket* socket = Portal::server->accept();

          ELLE_TRACE("new connection accepted");

          // allocate a new guest application.
          auto application = std::unique_ptr<Application>(new Application);

          // create the application.
          if (application->Create(socket) == elle::Status::Error)
            std::abort();

          // record the application.
          if (Portal::Add(application.get()) == elle::Status::Error)
            std::abort();

          application.release();
        }
    }

    // ///
    // /// this callback is triggered whenever an operation is requested by
    // /// an application.
    // ///
    // /// the role of this callback is to check that the application has
    // /// been authenticated.
    // ///
    // elle::Status        Portal::Prolog()
    // {
    //   Application*      application;

    //   // retrieve the application associated with the current socket.
    //   if (Portal::Retrieve(reactor::network::current_context().socket,
    //                        application) == elle::Status::Error)
    //     escape("unable to retrieve the application");

    //   // check if the application has been authenticated.
    //   if (application->state != Application::StateAuthenticated)
    //     escape("the application has not been authenticated");

    //   // set the application as processing.
    //   application->processing = Application::ProcessingOn;

    //   return elle::Status::Ok;
    // }

    // ///
    // /// this callback is triggered at the end of a portal invocation.
    // ///
    // /// the role of this callback is to reset the application's processing
    // /// but also to check if the application has been disconnected in
    // /// which case it must be removed and deleted.
    // ///
    // /// note that this callback lies in the application's socket. therefore,
    // /// the application deletion must be postponed in order to prevent the
    // /// system from executing code in a deleted instance.
    // ///
    // elle::Status        Portal::Epilog()
    // {
    //   Application*      application;

    //   // retrieve the application associated with the current socket.
    //   if (Portal::Retrieve(reactor::network::current_context().socket,
    //                        application) == elle::Status::Error)
    //     escape("unable to retrieve the application");

    //   // reset the application's processing.
    //   application->processing = Application::ProcessingOff;

    //   // check if the application's has been disconnected.
    //   if (application->state == Application::StateDisconnected)
    //     {
    //       // remove the application from the portal.
    //       if (Portal::Remove(application->socket) == elle::Status::Error)
    //         escape("unable to remove the application from the portal");

    //       // bury the application.
    //       bury(application);
    //     }

    //   return elle::Status::Ok;
    // }

  }
}
