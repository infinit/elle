//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [tue nov  1 12:32:18 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/portal/Portal.hh>

#include <etoile/wall/Wall.hh>

#include <etoile/Etoile.hh>

#include <Infinit.hh>

namespace etoile
{
  namespace portal
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// the line on which the portal listens for incoming connections.
    ///
    elle::String                Portal::Line;

    ///
    /// the container holding the connected applications.
    ///
    Portal::Container           Portal::Applications;

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the portal system.
    ///
    elle::Status        Portal::Initialize()
    {
      // build an arbitrary line for applications to connect to
      // the portal and issue requests.
      if (elle::Random::Generate(Portal::Line, 16) == elle::Status::Error)
        escape("unable to generate a random portal line");

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

        // register the message.
        if (elle::network::Network::Register(
              elle::network::Procedure<TagPathLocate,
                              TagPathWay>(
                elle::concurrency::Callback<>::Infer(&wall::Path::Locate),
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

      // listen for incoming connection.
      if (elle::LocalServer::Listen(
            Portal::Line,
            elle::concurrency::Callback<>::Infer(
              &Portal::Connection)) == elle::Status::Error)
        escape("unable to listen for local connections");

      return elle::Status::Ok;
    }

    ///
    /// this method cleans the portal system.
    ///
    elle::Status        Portal::Clean()
    {
      // nothing to do.

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
      if (Portal::Applications.find(application->socket) !=
          Portal::Applications.end())
        escape("this application seems to have already been registered");

      // insert the application in the container.
      result =
        Portal::Applications.insert(
          Portal::Value(application->socket, application));

      // check if the insertion was successful.
      if (result.second == false)
        escape("unable to insert the application in the container");

      return elle::Status::Ok;
    }

    ///
    /// this method returns the application associated with the given socket.
    ///
    elle::Status        Portal::Retrieve(elle::LocalSocket*     socket,
                                         Application*&          application)
    {
      Portal::Iterator  iterator;

      // locate the entry.
      if ((iterator =
           Portal::Applications.find(socket)) == Portal::Applications.end())
        escape("unable to locate the given socket");

      // return the application.
      application = iterator->second;

      return elle::Status::Ok;
    }

    ///
    /// this method removes an application from the container.
    ///
    elle::Status        Portal::Remove(elle::LocalSocket*       socket)
    {
      Portal::Iterator  iterator;

      // locate the entry.
      if ((iterator =
           Portal::Applications.find(socket)) == Portal::Applications.end())
        escape("unable to locate the given socket");

      // erase the entry.
      Portal::Applications.erase(iterator);

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
                << "[Line] " << Portal::Line << std::endl;

      // dump the applications.
      std::cout << alignment << elle::Dumpable::Shift
                << "[Applications]" << std::endl;

      // go through the applications.
      for (scoutor = Portal::Applications.begin();
           scoutor != Portal::Applications.end();
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

    ///
    /// this callback is triggered whenever a connection is made to etoile
    /// through the wall.
    ///
    elle::Status        Portal::Connection(elle::LocalSocket*   socket)
    {
      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        std::cout << "[etoile] portal::Portal::Connection()"
                  << std::endl;

      // allocate a new guest application.
      auto application = std::unique_ptr<Application>(new Application);

      // create the application.
      if (application->Create(socket) == elle::Status::Error)
        escape("unable to create the application");

      // record the application.
      if (Portal::Add(application.get()) == elle::Status::Error)
        escape("unable to add the new application");
      else
        application.release();

      return elle::Status::Ok;
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
      elle::network::Session*    session;
      Application*      application;

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        std::cout << "[etoile] portal::Portal::Authenticate()"
                  << std::endl;

      // retrieve the network session.
      if (elle::network::Session::Instance(session) == elle::Status::Error)
        escape("unable to retrieve the current session");

      // retrieve the application associated with the current socket.
      if (Portal::Retrieve(dynamic_cast<elle::LocalSocket*>(session->socket),
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
      elle::network::Session*    session;
      Application*      application;

      // retrieve the network session.
      if (elle::network::Session::Instance(session) == elle::Status::Error)
        escape("unable to retrieve the current session");

      // retrieve the application associated with the current socket.
      if (Portal::Retrieve(dynamic_cast<elle::LocalSocket*>(session->socket),
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
      elle::network::Session*    session;
      Application*      application;

      // retrieve the network session.
      if (elle::network::Session::Instance(session) == elle::Status::Error)
        escape("unable to retrieve the current session");

      // retrieve the application associated with the current socket.
      if (Portal::Retrieve(dynamic_cast<elle::LocalSocket*>(session->socket),
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
