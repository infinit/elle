#include <elle/log.hh>

#include <elle/cryptography/Random.hh>
#include <elle/network/Network.hh>
#include <elle/network/Procedure.hh>
#include <elle/standalone/Morgue.hh>

# include <reactor/thread.hh>
# include <reactor/network/tcp-server.hh>

#include <elle/network/Bundle.hh>
#include <elle/network/Header.hh>
#include <elle/standalone/Report.hh>
#include <elle/utility/Buffer.hh>

#include <nucleus/neutron/Trait.hh>
#include <nucleus/neutron/Range.hh>
#include <nucleus/neutron/Record.hh>
#include <nucleus/neutron/Entry.hh>

#include <etoile/path/Way.hh>
#include <etoile/path/Chemin.hh>
#include <etoile/gear/Identifier.hh>
#include <etoile/miscellaneous/Abstract.hh>

#include <etoile/portal/Portal.hh>

#include <etoile/wall/Wall.hh>

#include <etoile/Etoile.hh>

#include <Infinit.hh>

ELLE_LOG_TRACE_COMPONENT("etoile.portal.Portal");

namespace etoile
{
  namespace portal
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// the port on which the portal listens for incoming connections.
    ///
    elle::network::Port Portal::port = 12348; // XXX[to randomize]

    ///
    /// the container holding the connected applications.
    ///
    Portal::Container Portal::applications;

    // XXX
    reactor::network::TCPServer* Portal::server = nullptr;
    reactor::Thread* Portal::acceptor = nullptr;

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the portal system.
    ///
    elle::Status        Portal::Initialize()
    {
      ELLE_LOG_TRACE("register the messages");

      // register the messages.
      {
        //
        // general
        //

        // register the message.
        if (elle::network::Network::Register(
              elle::network::Procedure<TagAuthenticate>(
                elle::concurrency::Callback<>::Infer(
                  &Portal::Authenticate))) == elle::Status::Error)
          escape("unable to register the callback");

        //
        // path
        //

        // register the message.
        if (elle::network::Network::Register(
              elle::network::Procedure<TagPathResolve,
                              TagPathChemin>(
                elle::concurrency::Callback<>::Infer(&wall::Path::Resolve),
                elle::concurrency::Callback<>::Infer(&Portal::Prolog),
                elle::concurrency::Callback<>::Infer(&Portal::Epilog))) ==
            elle::Status::Error)
          escape("unable to register the callback");

        //
        // object
        //

        // register the message.
        if (elle::network::Network::Register(
              elle::network::Procedure<TagObjectLoad,
                              TagIdentifier>(
                elle::concurrency::Callback<>::Infer(&wall::Object::Load),
                elle::concurrency::Callback<>::Infer(&Portal::Prolog),
                elle::concurrency::Callback<>::Infer(&Portal::Epilog))) ==
            elle::Status::Error)
          escape("unable to register the callback");

        // register the message.
        if (elle::network::Network::Register(
              elle::network::Procedure<TagObjectInformation,
                              TagObjectAbstract>(
                elle::concurrency::Callback<>::Infer(&wall::Object::Information),
                elle::concurrency::Callback<>::Infer(&Portal::Prolog),
                elle::concurrency::Callback<>::Infer(&Portal::Epilog))) ==
              elle::Status::Error)
         escape("unable to register the callback");

        // register the message.
        if (elle::network::Network::Register(
              elle::network::Procedure<TagObjectDiscard,
                              elle::TagOk>(
                elle::concurrency::Callback<>::Infer(&wall::Object::Discard),
                elle::concurrency::Callback<>::Infer(&Portal::Prolog),
                elle::concurrency::Callback<>::Infer(&Portal::Epilog))) ==
              elle::Status::Error)
          escape("unable to register the callback");

        // register the message.
        if (elle::network::Network::Register(
              elle::network::Procedure<TagObjectStore,
                              elle::TagOk>(
                elle::concurrency::Callback<>::Infer(&wall::Object::Store),
                elle::concurrency::Callback<>::Infer(&Portal::Prolog),
                elle::concurrency::Callback<>::Infer(&Portal::Epilog))) ==
              elle::Status::Error)
          escape("unable to register the callback");

        // register the message.
        if (elle::network::Network::Register(
              elle::network::Procedure<TagObjectDestroy,
                              elle::TagOk>(
                elle::concurrency::Callback<>::Infer(&wall::Object::Destroy),
                elle::concurrency::Callback<>::Infer(&Portal::Prolog),
                elle::concurrency::Callback<>::Infer(&Portal::Epilog))) ==
              elle::Status::Error)
          escape("unable to register the callback");

        //
        // file
        //

        // register the message.
        if (elle::network::Network::Register(
              elle::network::Procedure<TagFileCreate,
                              TagIdentifier>(
                elle::concurrency::Callback<>::Infer(&wall::File::Create),
                elle::concurrency::Callback<>::Infer(&Portal::Prolog),
                elle::concurrency::Callback<>::Infer(&Portal::Epilog))) ==
              elle::Status::Error)
          escape("unable to register the callback");

        // register the message.
        if (elle::network::Network::Register(
              elle::network::Procedure<TagFileLoad,
                              TagIdentifier>(
                elle::concurrency::Callback<>::Infer(&wall::File::Load),
                elle::concurrency::Callback<>::Infer(&Portal::Prolog),
                elle::concurrency::Callback<>::Infer(&Portal::Epilog))) ==
              elle::Status::Error)
          escape("unable to register the callback");

        // register the message.
        if (elle::network::Network::Register(
              elle::network::Procedure<TagFileWrite,
                              elle::TagOk>(
                elle::concurrency::Callback<>::Infer( &wall::File::Write),
                elle::concurrency::Callback<>::Infer(&Portal::Prolog),
                elle::concurrency::Callback<>::Infer(&Portal::Epilog))) ==
              elle::Status::Error)
          escape("unable to register the callback");

        // register the message.
        if (elle::network::Network::Register(
              elle::network::Procedure<TagFileRead,
                              TagFileRegion>(
                elle::concurrency::Callback<>::Infer(&wall::File::Read),
                elle::concurrency::Callback<>::Infer(&Portal::Prolog),
                elle::concurrency::Callback<>::Infer(&Portal::Epilog))) ==
              elle::Status::Error)
          escape("unable to register the callback");

        // register the message.
        if (elle::network::Network::Register(
              elle::network::Procedure<TagFileAdjust,
                              elle::TagOk>(
                elle::concurrency::Callback<>::Infer(&wall::File::Adjust),
                elle::concurrency::Callback<>::Infer(&Portal::Prolog),
                elle::concurrency::Callback<>::Infer(&Portal::Epilog))) ==
              elle::Status::Error)
          escape("unable to register the callback");

        // register the message.
        if (elle::network::Network::Register(
              elle::network::Procedure<TagFileDiscard,
                              elle::TagOk>(
                elle::concurrency::Callback<>::Infer(&wall::File::Discard),
                elle::concurrency::Callback<>::Infer(&Portal::Prolog),
                elle::concurrency::Callback<>::Infer(&Portal::Epilog))) ==
              elle::Status::Error)
          escape("unable to register the callback");

        // register the message.
        if (elle::network::Network::Register(
              elle::network::Procedure<TagFileStore,
                              elle::TagOk>(
                elle::concurrency::Callback<>::Infer(&wall::File::Store),
                elle::concurrency::Callback<>::Infer(&Portal::Prolog),
                elle::concurrency::Callback<>::Infer(&Portal::Epilog))) ==
              elle::Status::Error)
          escape("unable to register the callback");

        // register the message.
        if (elle::network::Network::Register(
              elle::network::Procedure<TagFileDestroy,
                              elle::TagOk>(
                elle::concurrency::Callback<>::Infer(&wall::File::Destroy),
                elle::concurrency::Callback<>::Infer(&Portal::Prolog),
                elle::concurrency::Callback<>::Infer(&Portal::Epilog))) ==
              elle::Status::Error)
          escape("unable to register the callback");

        //
        // directory
        //

        // register the message.
        if (elle::network::Network::Register(
              elle::network::Procedure<TagDirectoryCreate,
                              TagIdentifier>(
                elle::concurrency::Callback<>::Infer(&wall::Directory::Create),
                elle::concurrency::Callback<>::Infer(&Portal::Prolog),
                elle::concurrency::Callback<>::Infer(&Portal::Epilog))) ==
              elle::Status::Error)
          escape("unable to register the callback");

        // register the message.
        if (elle::network::Network::Register(
              elle::network::Procedure<TagDirectoryLoad,
                              TagIdentifier>(
                elle::concurrency::Callback<>::Infer(&wall::Directory::Load),
                elle::concurrency::Callback<>::Infer(&Portal::Prolog),
                elle::concurrency::Callback<>::Infer(&Portal::Epilog))) ==
              elle::Status::Error)
          escape("unable to register the callback");

        // register the message.
        if (elle::network::Network::Register(
              elle::network::Procedure<TagDirectoryAdd,
                              elle::TagOk>(
                elle::concurrency::Callback<>::Infer(&wall::Directory::Add),
                elle::concurrency::Callback<>::Infer(&Portal::Prolog),
                elle::concurrency::Callback<>::Infer(&Portal::Epilog))) ==
              elle::Status::Error)
          escape("unable to register the callback");

        // register the message.
        if (elle::network::Network::Register(
              elle::network::Procedure<TagDirectoryLookup,
                              TagDirectoryEntry>(
                elle::concurrency::Callback<>::Infer(&Wrapper::Directory::Lookup),
                elle::concurrency::Callback<>::Infer(&Portal::Prolog),
                elle::concurrency::Callback<>::Infer(&Portal::Epilog))) ==
              elle::Status::Error)
          escape("unable to register the callback");

        // register the message.
        if (elle::network::Network::Register(
              elle::network::Procedure<TagDirectoryConsult,
                              TagDirectoryRange>(
                elle::concurrency::Callback<>::Infer(&Wrapper::Directory::Consult),
                elle::concurrency::Callback<>::Infer(&Portal::Prolog),
                elle::concurrency::Callback<>::Infer(&Portal::Epilog))) ==
              elle::Status::Error)
          escape("unable to register the callback");

        // register the message.
        if (elle::network::Network::Register(
              elle::network::Procedure<TagDirectoryRename,
                              elle::TagOk>(
                elle::concurrency::Callback<>::Infer(&wall::Directory::Rename),
                elle::concurrency::Callback<>::Infer(&Portal::Prolog),
                elle::concurrency::Callback<>::Infer(&Portal::Epilog))) ==
              elle::Status::Error)
          escape("unable to register the callback");

        // register the message.
        if (elle::network::Network::Register(
              elle::network::Procedure<TagDirectoryRemove,
                              elle::TagOk>(
                elle::concurrency::Callback<>::Infer(&wall::Directory::Remove),
                elle::concurrency::Callback<>::Infer(&Portal::Prolog),
                elle::concurrency::Callback<>::Infer(&Portal::Epilog))) ==
              elle::Status::Error)
          escape("unable to register the callback");

        // register the message.
        if (elle::network::Network::Register(
              elle::network::Procedure<TagDirectoryDiscard,
                              elle::TagOk>(
                elle::concurrency::Callback<>::Infer(&wall::Directory::Discard),
                elle::concurrency::Callback<>::Infer(&Portal::Prolog),
                elle::concurrency::Callback<>::Infer(&Portal::Epilog))) ==
              elle::Status::Error)
          escape("unable to register the callback");

        // register the message.
        if (elle::network::Network::Register(
              elle::network::Procedure<TagDirectoryStore,
                              elle::TagOk>(
                elle::concurrency::Callback<>::Infer(&wall::Directory::Store),
                elle::concurrency::Callback<>::Infer(&Portal::Prolog),
                elle::concurrency::Callback<>::Infer(&Portal::Epilog))) ==
              elle::Status::Error)
          escape("unable to register the callback");

        // register the message.
        if (elle::network::Network::Register(
              elle::network::Procedure<TagDirectoryDestroy,
                              elle::TagOk>(
                elle::concurrency::Callback<>::Infer(&wall::Directory::Destroy),
                elle::concurrency::Callback<>::Infer(&Portal::Prolog),
                elle::concurrency::Callback<>::Infer(&Portal::Epilog))) ==
              elle::Status::Error)
          escape("unable to register the callback");

        //
        // link
        //

        // register the message.
        if (elle::network::Network::Register(
              elle::network::Procedure<TagLinkCreate,
                              TagIdentifier>(
                elle::concurrency::Callback<>::Infer(&wall::Link::Create),
                elle::concurrency::Callback<>::Infer(&Portal::Prolog),
                elle::concurrency::Callback<>::Infer(&Portal::Epilog))) ==
              elle::Status::Error)
          escape("unable to register the callback");

        // register the message.
        if (elle::network::Network::Register(
              elle::network::Procedure<TagLinkLoad,
                              TagIdentifier>(
                elle::concurrency::Callback<>::Infer(&wall::Link::Load),
                elle::concurrency::Callback<>::Infer(&Portal::Prolog),
                elle::concurrency::Callback<>::Infer(&Portal::Epilog))) ==
              elle::Status::Error)
          escape("unable to register the callback");

        // register the message.
        if (elle::network::Network::Register(
              elle::network::Procedure<TagLinkBind,
                              elle::TagOk>(
                elle::concurrency::Callback<>::Infer(&wall::Link::Bind),
                elle::concurrency::Callback<>::Infer(&Portal::Prolog),
                elle::concurrency::Callback<>::Infer(&Portal::Epilog))) ==
              elle::Status::Error)
          escape("unable to register the callback");

        // register the message.
        if (elle::network::Network::Register(
              elle::network::Procedure<TagLinkResolve,
                              TagLinkWay>(
                elle::concurrency::Callback<>::Infer(&wall::Link::Resolve),
                elle::concurrency::Callback<>::Infer(&Portal::Prolog),
                elle::concurrency::Callback<>::Infer(&Portal::Epilog))) ==
              elle::Status::Error)
          escape("unable to register the callback");

        // register the message.
        if (elle::network::Network::Register(
              elle::network::Procedure<TagLinkDiscard,
                              elle::TagOk>(
                elle::concurrency::Callback<>::Infer(&wall::Link::Discard),
                elle::concurrency::Callback<>::Infer(&Portal::Prolog),
                elle::concurrency::Callback<>::Infer(&Portal::Epilog))) ==
              elle::Status::Error)
          escape("unable to register the callback");

        // register the message.
        if (elle::network::Network::Register(
              elle::network::Procedure<TagLinkStore,
                              elle::TagOk>(
                elle::concurrency::Callback<>::Infer(&wall::Link::Store),
                elle::concurrency::Callback<>::Infer(&Portal::Prolog),
                elle::concurrency::Callback<>::Infer(&Portal::Epilog))) ==
              elle::Status::Error)
          escape("unable to register the callback");

        // register the message.
        if (elle::network::Network::Register(
              elle::network::Procedure<TagLinkDestroy,
                              elle::TagOk>(
                elle::concurrency::Callback<>::Infer(&wall::Link::Destroy),
                elle::concurrency::Callback<>::Infer(&Portal::Prolog),
                elle::concurrency::Callback<>::Infer(&Portal::Epilog))) ==
              elle::Status::Error)
          escape("unable to register the callback");

        //
        // access
        //

        // register the message.
        if (elle::network::Network::Register(
              elle::network::Procedure<TagAccessLookup,
                              TagAccessRecord>(
                elle::concurrency::Callback<>::Infer(&Wrapper::Access::Lookup),
                elle::concurrency::Callback<>::Infer(&Portal::Prolog),
                elle::concurrency::Callback<>::Infer(&Portal::Epilog))) ==
              elle::Status::Error)
          escape("unable to register the callback");

        // register the message.
        if (elle::network::Network::Register(
              elle::network::Procedure<TagAccessConsult,
                              TagAccessRange>(
                elle::concurrency::Callback<>::Infer(&Wrapper::Access::Consult),
                elle::concurrency::Callback<>::Infer(&Portal::Prolog),
                elle::concurrency::Callback<>::Infer(&Portal::Epilog))) ==
              elle::Status::Error)
          escape("unable to register the callback");

        // register the message.
        if (elle::network::Network::Register(
              elle::network::Procedure<TagAccessGrant,
                              elle::TagOk>(
                elle::concurrency::Callback<>::Infer(&wall::Access::Grant),
                elle::concurrency::Callback<>::Infer(&Portal::Prolog),
                elle::concurrency::Callback<>::Infer(&Portal::Epilog))) ==
              elle::Status::Error)
          escape("unable to register the callback");

        // register the message.
        if (elle::network::Network::Register(
              elle::network::Procedure<TagAccessRevoke,
                              elle::TagOk>(
                elle::concurrency::Callback<>::Infer(&wall::Access::Revoke),
                elle::concurrency::Callback<>::Infer(&Portal::Prolog),
                elle::concurrency::Callback<>::Infer(&Portal::Epilog))) ==
              elle::Status::Error)
          escape("unable to register the callback");

        //
        // attributes
        //

        // register the message.
        if (elle::network::Network::Register(
              elle::network::Procedure<TagAttributesSet,
                              elle::TagOk>(
                elle::concurrency::Callback<>::Infer(&wall::Attributes::Set),
                elle::concurrency::Callback<>::Infer(&Portal::Prolog),
                elle::concurrency::Callback<>::Infer(&Portal::Epilog))) ==
              elle::Status::Error)
          escape("unable to register the callback");

        // register the message.
        if (elle::network::Network::Register(
              elle::network::Procedure<TagAttributesGet,
                              TagAttributesTrait>(
                elle::concurrency::Callback<>::Infer(&Wrapper::Attributes::Get),
                elle::concurrency::Callback<>::Infer(&Portal::Prolog),
                elle::concurrency::Callback<>::Infer(&Portal::Epilog))) ==
              elle::Status::Error)
          escape("unable to register the callback");

        // register the message.
        if (elle::network::Network::Register(
              elle::network::Procedure<TagAttributesFetch,
                              TagAttributesRange>(
                elle::concurrency::Callback<>::Infer(&Wrapper::Attributes::Fetch),
                elle::concurrency::Callback<>::Infer(&Portal::Prolog),
                elle::concurrency::Callback<>::Infer(&Portal::Epilog))) ==
              elle::Status::Error)
          escape("unable to register the callback");

        // register the message.
        if (elle::network::Network::Register(
              elle::network::Procedure<TagAttributesOmit,
                              elle::TagOk>(
                elle::concurrency::Callback<>::Infer(&wall::Attributes::Omit),
                elle::concurrency::Callback<>::Infer(&Portal::Prolog),
                elle::concurrency::Callback<>::Infer(&Portal::Epilog))) ==
              elle::Status::Error)
          escape("unable to register the callback");
      }

      try
        {
          ELLE_LOG_TRACE("set up the server and allocator");

          // allocate the server and acceptor for handling incoming connections.
          Portal::server =
            new reactor::network::TCPServer(elle::concurrency::scheduler());
          Portal::acceptor =
            new reactor::Thread(elle::concurrency::scheduler(),
                                "Portal Server accept",
                                boost::bind(&Portal::accept));

          ELLE_LOG_TRACE("listen");

          // finally, listen for incoming connections.
          Portal::server->listen(Portal::port);
        }
      catch (std::runtime_error& e)
        {
          escape("unable to set up the portal server: %s",
                 e.what());
        }

      //
      // generate a phrase randomly which will be used by applications to
      // connect to Etoile and trigger specific actions.
      //
      // generate a random string, create a phrase with it along with
      // the socket used by portal so that applications have everything
      // to connect to and authenticate to portal.
      if (!Infinit::Network.empty())
        {
          elle::String pass;

          if (elle::cryptography::Random::Generate(pass) == elle::Status::Error)
            escape("unable to generate a random string");

          if (Etoile::Phrase.Create(Portal::port,
                                    pass) == elle::Status::Error)
            escape("unable to create the phrase");

          if (Etoile::Phrase.Store(Infinit::Network) == elle::Status::Error)
            escape("unable to store the phrase");
        }

      return elle::Status::Ok;
    }

    ///
    /// this method cleans the portal system.
    ///
    elle::Status        Portal::Clean()
    {
      Portal::Scoutor scoutor;

      // delete the phrase.
      if (!Infinit::Network.empty())
        {
          if (Etoile::Phrase.Erase(Infinit::Network) == elle::Status::Error)
            escape("unable to erase the phrase");
        }

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
    /// this method returns the application associated with the given socket.
    ///
    elle::Status        Portal::Retrieve(elle::network::TCPSocket*     socket,
                                         Application*&          application)
    {
      Portal::Iterator  iterator;

      // locate the entry.
      if ((iterator =
           Portal::applications.find(socket)) == Portal::applications.end())
        escape("unable to locate the given socket");

      // return the application.
      application = iterator->second;

      return elle::Status::Ok;
    }

    ///
    /// this method removes an application from the container.
    ///
    elle::Status        Portal::Remove(elle::network::TCPSocket*       socket)
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

      // dump the line.
      std::cout << alignment << elle::Dumpable::Shift
                << "[Port] " << Portal::port << std::endl;

      // dump the applications.
      std::cout << alignment << elle::Dumpable::Shift
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

            ELLE_LOG_TRACE("new connection accepted");

            // allocate a new guest application.
            auto application = std::unique_ptr<Application>(new Application);

            // create the application.
            if (application->Create(new elle::network::TCPSocket(socket)) == elle::Status::Error)
              std::abort();

            // record the application.
            if (Portal::Add(application.get()) == elle::Status::Error)
              std::abort();

            application.release();
          }
      }

    ///
    /// this callback is triggered whenever the portal receives an
    /// Authenticate message from an already connected application.
    ///
    /// this message comes with a phrase which is compared to the current
    /// instance's. if the phrase is valid, the application is authorised
    /// to trigger operation on behalf of the current user.
    ///
    elle::Status        Portal::Authenticate(const elle::String&        pass)
    {
      Application*      application;

      ELLE_LOG_TRACE("Authenticate()");

      // retrieve the application associated with the current socket.
      if (Portal::Retrieve(elle::network::current_context().socket,
                           application) == elle::Status::Error)
        escape("unable to retrieve the application");

      // check that the given phrase is the same as etoile's.
      if (Etoile::Phrase.pass != pass)
        escape("the given pass is invalid");

      // set the application as being authenticated.
      application->state = Application::StateAuthenticated;

      // reply with the Authenticated message.
      if (application->socket->Reply(
            elle::network::Inputs<TagAuthenticated>()) == elle::Status::Error)
        escape("unable to reply to the application");

      return elle::Status::Ok;
    }

    ///
    /// this callback is triggered whenever an operation is requested by
    /// an application.
    ///
    /// the role of this callback is to check that the application has
    /// been authenticated.
    ///
    elle::Status        Portal::Prolog()
    {
      Application*      application;

      // retrieve the application associated with the current socket.
      if (Portal::Retrieve(elle::network::current_context().socket,
                           application) == elle::Status::Error)
        escape("unable to retrieve the application");

      // check if the application has been authenticated.
      if (application->state != Application::StateAuthenticated)
        escape("the application has not been authenticated");

      // set the application as processing.
      application->processing = Application::ProcessingOn;

      return elle::Status::Ok;
    }

    ///
    /// this callback is triggered at the end of a portal invocation.
    ///
    /// the role of this callback is to reset the application's processing
    /// but also to check if the application has been disconnected in
    /// which case it must be removed and deleted.
    ///
    /// note that this callback lies in the application's socket. therefore,
    /// the application deletion must be postponed in order to prevent the
    /// system from executing code in a deleted instance.
    ///
    elle::Status        Portal::Epilog()
    {
      Application*      application;

      // retrieve the application associated with the current socket.
      if (Portal::Retrieve(elle::network::current_context().socket,
                           application) == elle::Status::Error)
        escape("unable to retrieve the application");

      // reset the application's processing.
      application->processing = Application::ProcessingOff;

      // check if the application's has been disconnected.
      if (application->state == Application::StateDisconnected)
        {
          // remove the application from the portal.
          if (Portal::Remove(application->socket) == elle::Status::Error)
            escape("unable to remove the application from the portal");

          // bury the application.
          bury(application);
        }

      return elle::Status::Ok;
    }

  }
}
