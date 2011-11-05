//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien.quintard   [tue nov  1 12:32:18 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/portal/Portal.hh>

#include <etoile/wall/Wall.hh>

#include <etoile/Etoile.hh>

#include <agent/Agent.hh>
#include <hole/Hole.hh>
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
    elle::String		Portal::Line;

    ///
    /// the container holding the connected applications.
    ///
    Portal::Container		Portal::Applications;

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the portal system.
    ///
    elle::Status	Portal::Initialize()
    {
      enter();

      // build the line depending on both the user and network names.
      Portal::Line =
	"etoile-" +
	agent::Agent::Identity.name +
	":" +
	hole::Hole::Descriptor.name;

      /// XXX \todo le faire proprement en quittant, tout nettoyer, notamment
      ///     en catchant les signaux (UNIX/QT) et via unlink.
      elle::String	path = "/tmp/" + Portal::Line;
      ::unlink(path.c_str());

      // register the messages.
      {
	//
	// general
	//

	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagAuthenticate>(
		elle::Callback<>::Infer(
		  &Portal::Authenticate))) == elle::StatusError)
	  escape("unable to register the callback");

	//
	// path
	//

	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagPathResolve,
			      TagPathChemin>(
	        elle::Callback<>::Infer(&wall::Path::Resolve),
		elle::Callback<>::Infer(&Portal::Prolog),
		elle::Callback<>::Infer(&Portal::Epilog))) ==
	    elle::StatusError)
	  escape("unable to register the callback");

	//
	// object
	//

	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagObjectLoad,
			      TagIdentifier>(
	        elle::Callback<>::Infer(&wall::Object::Load),
		elle::Callback<>::Infer(&Portal::Prolog),
		elle::Callback<>::Infer(&Portal::Epilog))) ==
	    elle::StatusError)
	  escape("unable to register the callback");

	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagObjectInformation,
			      TagObjectAbstract>(
		elle::Callback<>::Infer(&wall::Object::Information),
		elle::Callback<>::Infer(&Portal::Prolog),
		elle::Callback<>::Infer(&Portal::Epilog))) ==
	      elle::StatusError)
	  escape("unable to register the callback");

	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagObjectDiscard,
			      elle::TagOk>(
	        elle::Callback<>::Infer(&wall::Object::Discard),
		elle::Callback<>::Infer(&Portal::Prolog),
		elle::Callback<>::Infer(&Portal::Epilog))) ==
	      elle::StatusError)
	  escape("unable to register the callback");

	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagObjectStore,
			      elle::TagOk>(
	        elle::Callback<>::Infer(&wall::Object::Store),
		elle::Callback<>::Infer(&Portal::Prolog),
		elle::Callback<>::Infer(&Portal::Epilog))) ==
	      elle::StatusError)
	  escape("unable to register the callback");

	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagObjectDestroy,
			      elle::TagOk>(
	        elle::Callback<>::Infer(&wall::Object::Destroy),
		elle::Callback<>::Infer(&Portal::Prolog),
		elle::Callback<>::Infer(&Portal::Epilog))) ==
	      elle::StatusError)
	  escape("unable to register the callback");

	//
	// file
	//

	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagFileCreate,
			      TagIdentifier>(
	        elle::Callback<>::Infer(&wall::File::Create),
		elle::Callback<>::Infer(&Portal::Prolog),
		elle::Callback<>::Infer(&Portal::Epilog))) ==
	      elle::StatusError)
	  escape("unable to register the callback");

	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagFileLoad,
			      TagIdentifier>(
	        elle::Callback<>::Infer(&wall::File::Load),
		elle::Callback<>::Infer(&Portal::Prolog),
		elle::Callback<>::Infer(&Portal::Epilog))) ==
	      elle::StatusError)
	  escape("unable to register the callback");

	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagFileWrite,
			      elle::TagOk>(
	        elle::Callback<>::Infer( &wall::File::Write),
		elle::Callback<>::Infer(&Portal::Prolog),
		elle::Callback<>::Infer(&Portal::Epilog))) ==
	      elle::StatusError)
	  escape("unable to register the callback");

	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagFileRead,
			      TagFileRegion>(
	        elle::Callback<>::Infer(&wall::File::Read),
		elle::Callback<>::Infer(&Portal::Prolog),
		elle::Callback<>::Infer(&Portal::Epilog))) ==
	      elle::StatusError)
	  escape("unable to register the callback");

	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagFileAdjust,
			      elle::TagOk>(
	        elle::Callback<>::Infer(&wall::File::Adjust),
		elle::Callback<>::Infer(&Portal::Prolog),
		elle::Callback<>::Infer(&Portal::Epilog))) ==
	      elle::StatusError)
	  escape("unable to register the callback");

	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagFileDiscard,
			      elle::TagOk>(
	        elle::Callback<>::Infer(&wall::File::Discard),
		elle::Callback<>::Infer(&Portal::Prolog),
		elle::Callback<>::Infer(&Portal::Epilog))) ==
	      elle::StatusError)
	  escape("unable to register the callback");

	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagFileStore,
			      elle::TagOk>(
	        elle::Callback<>::Infer(&wall::File::Store),
		elle::Callback<>::Infer(&Portal::Prolog),
		elle::Callback<>::Infer(&Portal::Epilog))) ==
	      elle::StatusError)
	  escape("unable to register the callback");

	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagFileDestroy,
			      elle::TagOk>(
	        elle::Callback<>::Infer(&wall::File::Destroy),
		elle::Callback<>::Infer(&Portal::Prolog),
		elle::Callback<>::Infer(&Portal::Epilog))) ==
	      elle::StatusError)
	  escape("unable to register the callback");

	//
	// directory
	//

	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagDirectoryCreate,
			      TagIdentifier>(
	        elle::Callback<>::Infer(&wall::Directory::Create),
		elle::Callback<>::Infer(&Portal::Prolog),
		elle::Callback<>::Infer(&Portal::Epilog))) ==
	      elle::StatusError)
	  escape("unable to register the callback");

	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagDirectoryLoad,
			      TagIdentifier>(
	        elle::Callback<>::Infer(&wall::Directory::Load),
		elle::Callback<>::Infer(&Portal::Prolog),
		elle::Callback<>::Infer(&Portal::Epilog))) ==
	      elle::StatusError)
	  escape("unable to register the callback");

	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagDirectoryAdd,
			      elle::TagOk>(
	        elle::Callback<>::Infer(&wall::Directory::Add),
		elle::Callback<>::Infer(&Portal::Prolog),
		elle::Callback<>::Infer(&Portal::Epilog))) ==
	      elle::StatusError)
	  escape("unable to register the callback");

	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagDirectoryLookup,
			      TagDirectoryEntry>(
	        elle::Callback<>::Infer(&Wrapper::Directory::Lookup),
		elle::Callback<>::Infer(&Portal::Prolog),
		elle::Callback<>::Infer(&Portal::Epilog))) ==
	      elle::StatusError)
	  escape("unable to register the callback");

	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagDirectoryConsult,
			      TagDirectoryRange>(
	        elle::Callback<>::Infer(&Wrapper::Directory::Consult),
		elle::Callback<>::Infer(&Portal::Prolog),
		elle::Callback<>::Infer(&Portal::Epilog))) ==
	      elle::StatusError)
	  escape("unable to register the callback");

	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagDirectoryRename,
			      elle::TagOk>(
	        elle::Callback<>::Infer(&wall::Directory::Rename),
		elle::Callback<>::Infer(&Portal::Prolog),
		elle::Callback<>::Infer(&Portal::Epilog))) ==
	      elle::StatusError)
	  escape("unable to register the callback");

	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagDirectoryRemove,
			      elle::TagOk>(
	        elle::Callback<>::Infer(&wall::Directory::Remove),
		elle::Callback<>::Infer(&Portal::Prolog),
		elle::Callback<>::Infer(&Portal::Epilog))) ==
	      elle::StatusError)
	  escape("unable to register the callback");

	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagDirectoryDiscard,
			      elle::TagOk>(
	        elle::Callback<>::Infer(&wall::Directory::Discard),
		elle::Callback<>::Infer(&Portal::Prolog),
		elle::Callback<>::Infer(&Portal::Epilog))) ==
	      elle::StatusError)
	  escape("unable to register the callback");

	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagDirectoryStore,
			      elle::TagOk>(
	        elle::Callback<>::Infer(&wall::Directory::Store),
		elle::Callback<>::Infer(&Portal::Prolog),
		elle::Callback<>::Infer(&Portal::Epilog))) ==
	      elle::StatusError)
	  escape("unable to register the callback");

	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagDirectoryDestroy,
			      elle::TagOk>(
	        elle::Callback<>::Infer(&wall::Directory::Destroy),
		elle::Callback<>::Infer(&Portal::Prolog),
		elle::Callback<>::Infer(&Portal::Epilog))) ==
	      elle::StatusError)
	  escape("unable to register the callback");

	//
	// link
	//

	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagLinkCreate,
			      TagIdentifier>(
	        elle::Callback<>::Infer(&wall::Link::Create),
		elle::Callback<>::Infer(&Portal::Prolog),
		elle::Callback<>::Infer(&Portal::Epilog))) ==
	      elle::StatusError)
	  escape("unable to register the callback");

	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagLinkLoad,
			      TagIdentifier>(
	        elle::Callback<>::Infer(&wall::Link::Load),
		elle::Callback<>::Infer(&Portal::Prolog),
		elle::Callback<>::Infer(&Portal::Epilog))) ==
	      elle::StatusError)
	  escape("unable to register the callback");

	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagLinkBind,
			      elle::TagOk>(
	        elle::Callback<>::Infer(&wall::Link::Bind),
		elle::Callback<>::Infer(&Portal::Prolog),
		elle::Callback<>::Infer(&Portal::Epilog))) ==
	      elle::StatusError)
	  escape("unable to register the callback");

	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagLinkResolve,
			      TagLinkWay>(
	        elle::Callback<>::Infer(&wall::Link::Resolve),
		elle::Callback<>::Infer(&Portal::Prolog),
		elle::Callback<>::Infer(&Portal::Epilog))) ==
	      elle::StatusError)
	  escape("unable to register the callback");

	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagLinkDiscard,
			      elle::TagOk>(
	        elle::Callback<>::Infer(&wall::Link::Discard),
		elle::Callback<>::Infer(&Portal::Prolog),
		elle::Callback<>::Infer(&Portal::Epilog))) ==
	      elle::StatusError)
	  escape("unable to register the callback");

	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagLinkStore,
			      elle::TagOk>(
	        elle::Callback<>::Infer(&wall::Link::Store),
		elle::Callback<>::Infer(&Portal::Prolog),
		elle::Callback<>::Infer(&Portal::Epilog))) ==
	      elle::StatusError)
	  escape("unable to register the callback");

	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagLinkDestroy,
			      elle::TagOk>(
	        elle::Callback<>::Infer(&wall::Link::Destroy),
		elle::Callback<>::Infer(&Portal::Prolog),
		elle::Callback<>::Infer(&Portal::Epilog))) ==
	      elle::StatusError)
	  escape("unable to register the callback");

	//
	// access
	//

	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagAccessLookup,
			      TagAccessRecord>(
	        elle::Callback<>::Infer(&Wrapper::Access::Lookup),
		elle::Callback<>::Infer(&Portal::Prolog),
		elle::Callback<>::Infer(&Portal::Epilog))) ==
	      elle::StatusError)
	  escape("unable to register the callback");

	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagAccessConsult,
			      TagAccessRange>(
	        elle::Callback<>::Infer(&Wrapper::Access::Consult),
		elle::Callback<>::Infer(&Portal::Prolog),
		elle::Callback<>::Infer(&Portal::Epilog))) ==
	      elle::StatusError)
	  escape("unable to register the callback");

	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagAccessGrant,
			      elle::TagOk>(
	        elle::Callback<>::Infer(&wall::Access::Grant),
		elle::Callback<>::Infer(&Portal::Prolog),
		elle::Callback<>::Infer(&Portal::Epilog))) ==
	      elle::StatusError)
	  escape("unable to register the callback");

	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagAccessRevoke,
			      elle::TagOk>(
	        elle::Callback<>::Infer(&wall::Access::Revoke),
		elle::Callback<>::Infer(&Portal::Prolog),
		elle::Callback<>::Infer(&Portal::Epilog))) ==
	      elle::StatusError)
	  escape("unable to register the callback");

	//
	// attributes
	//

	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagAttributesSet,
			      elle::TagOk>(
	        elle::Callback<>::Infer(&wall::Attributes::Set),
		elle::Callback<>::Infer(&Portal::Prolog),
		elle::Callback<>::Infer(&Portal::Epilog))) ==
	      elle::StatusError)
	  escape("unable to register the callback");

	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagAttributesGet,
			      TagAttributesTrait>(
	        elle::Callback<>::Infer(&Wrapper::Attributes::Get),
		elle::Callback<>::Infer(&Portal::Prolog),
		elle::Callback<>::Infer(&Portal::Epilog))) ==
	      elle::StatusError)
	  escape("unable to register the callback");

	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagAttributesFetch,
			      TagAttributesRange>(
	        elle::Callback<>::Infer(&Wrapper::Attributes::Fetch),
		elle::Callback<>::Infer(&Portal::Prolog),
		elle::Callback<>::Infer(&Portal::Epilog))) ==
	      elle::StatusError)
	  escape("unable to register the callback");

	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagAttributesOmit,
			      elle::TagOk>(
	        elle::Callback<>::Infer(&wall::Attributes::Omit),
		elle::Callback<>::Infer(&Portal::Prolog),
		elle::Callback<>::Infer(&Portal::Epilog))) ==
	      elle::StatusError)
	  escape("unable to register the callback");
      }

      // listen for incoming connection.
      if (elle::Lane::Listen(
	    Portal::Line,
	    elle::Callback<>::Infer(
	      &Portal::Connection)) == elle::StatusError)
        escape("unable to listen for lane connections");

      leave();
    }

    ///
    /// this method cleans the portal system.
    ///
    elle::Status	Portal::Clean()
    {
      enter();

      /// XXX \todo le faire proprement en quittant, tout nettoyer, notamment
      ///     en catchant les signaux (UNIX/QT) et via unlink.
      elle::String	path = "/tmp/" + Portal::Line;
      ::unlink(path.c_str());

      leave();
    }

    ///
    /// this method registers an application in the set of connected
    /// applications.
    ///
    elle::Status	Portal::Add(Application*		application)
    {
      std::pair<Portal::Iterator, elle::Boolean>	result;

      enter();

      // check if this application has already been registered.
      if (Portal::Applications.find(application->door) !=
	  Portal::Applications.end())
        escape("this application seems to have already been registered");

      // insert the application in the container.
      result =
	Portal::Applications.insert(
	  Portal::Value(application->door, application));

      // check if the insertion was successful.
      if (result.second == false)
        escape("unable to insert the application in the container");

      leave();
    }

    ///
    /// this method returns the application associated with the given door.
    ///
    elle::Status	Portal::Retrieve(elle::Door*		door,
					 Application*&		application)
    {
      Portal::Iterator	iterator;

      enter();

      // locate the entry.
      if ((iterator =
	   Portal::Applications.find(door)) == Portal::Applications.end())
	escape("unable to locate the given door");

      // return the application.
      application = iterator->second;

      leave();
    }

    ///
    /// this method removes an application from the container.
    ///
    elle::Status	Portal::Remove(elle::Door*		door)
    {
      Portal::Iterator	iterator;

      enter();

      // locate the entry.
      if ((iterator =
	   Portal::Applications.find(door)) == Portal::Applications.end())
	escape("unable to locate the given door");

      // erase the entry.
      Portal::Applications.erase(iterator);

      leave();
    }

    ///
    /// this method dumps the whole portal.
    ///
    elle::Status	Portal::Show(const elle::Natural32	margin)
    {
      elle::String	alignment(margin, ' ');
      Portal::Scoutor	scoutor;

      enter();

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
	  Application*	application = scoutor->second;

	  // dump the application.
	  if (application->Dump(margin + 4) == elle::StatusError)
	    escape("unable to dump the application");
	}

      leave();
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this callback is triggered whenever a connection is made to etoile
    /// through the wall.
    ///
    elle::Status	Portal::Connection(elle::Door*		door)
    {
      Application*	application;

      enterx(instance(application));

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
	std::cout << "[etoile] portal::Portal::Connection()"
		  << std::endl;

      // allocate a new guest application.
      application = new Application;

      // create the application.
      if (application->Create(door) == elle::StatusError)
        escape("unable to create the application");

      // record the application.
      if (Portal::Add(application) == elle::StatusError)
        escape("unable to add the new application");

      // stop tracking the application.
      waive(application);

      leave();
    }

    ///
    /// this callback is triggered whenever the portal receives an
    /// Authenticate message from an already connected application.
    ///
    /// this message comes with a phrase which is compared to the current
    /// instance's. if the phrase is valid, the application is authorised
    /// to trigger operation on behalf of the current user.
    ///
    elle::Status	Portal::Authenticate(const lune::Phrase& phrase)
    {
      elle::Session*	session;
      Application*	application;

      enter();

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
	std::cout << "[etoile] portal::Portal::Authenticate()"
		  << std::endl;

      // retrieve the network session.
      if (elle::Session::Instance(session) == elle::StatusError)
	escape("unable to retrieve the current session");

      // retrieve the application associated with the current socket.
      if (Portal::Retrieve(dynamic_cast<elle::Door*>(session->socket),
			   application) == elle::StatusError)
	escape("unable to retrieve the application");

      // check that the given phrase is the same as etoile's.
      if (Etoile::Phrase != phrase)
	escape("the given phrase is invalid");

      // set the application as being authenticated.
      application->state = Application::StateAuthenticated;

      // reply with the Authenticated message.
      if (application->door->Reply(
	    elle::Inputs<TagAuthenticated>()) == elle::StatusError)
	escape("unable to reply to the application");

      leave();
    }

    ///
    /// this callback is triggered whenever an operation is requested by
    /// an application.
    ///
    /// the role of this callback is to check that the application has
    /// been authenticated.
    ///
    elle::Status	Portal::Prolog()
    {
      elle::Session*	session;
      Application*	application;

      enter();

      // retrieve the network session.
      if (elle::Session::Instance(session) == elle::StatusError)
	escape("unable to retrieve the current session");

      // retrieve the application associated with the current socket.
      if (Portal::Retrieve(dynamic_cast<elle::Door*>(session->socket),
			   application) == elle::StatusError)
	escape("unable to retrieve the application");

      // check if the application has been authenticated.
      if (application->state != Application::StateAuthenticated)
	escape("the application has not been authenticated");

      // set the application as processing.
      application->processing = Application::ProcessingOn;

      leave();
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
    elle::Status	Portal::Epilog()
    {
      elle::Session*	session;
      Application*	application;

      enter();

      // retrieve the network session.
      if (elle::Session::Instance(session) == elle::StatusError)
	escape("unable to retrieve the current session");

      // retrieve the application associated with the current socket.
      if (Portal::Retrieve(dynamic_cast<elle::Door*>(session->socket),
			   application) == elle::StatusError)
	escape("unable to retrieve the application");

      // reset the application's processing.
      application->processing = Application::ProcessingOff;

      // check if the application's has been disconnected.
      if (application->state == Application::StateDisconnected)
	{
      	  // remove the application from the portal.
	  if (Portal::Remove(application->door) == elle::StatusError)
	    escape("unable to remove the application from the portal");

	  // bury the application.
	  bury(application);
	}

      leave();
    }

  }
}
