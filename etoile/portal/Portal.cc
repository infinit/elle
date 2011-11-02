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
    elle::String		Portal::Line("etoile");

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
	// access
	//

	/* XXX
	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagAccessLookup,
			      TagAccessRecord>(
	        elle::Callback<>::Infer(
		  &wall::Access::Lookup))) == elle::StatusError)
	  escape("unable to register the callback");
	*/
	// XXX
	/*
	// register the message.
	if (elle::Network::Register(
	      elle::Procedure<TagDirectoryLookup,
			      TagDirectoryEntry>(
	        elle::Callback<>::Infer(
		  &wall::Directory::Lookup))) == elle::StatusError)
	  escape("unable to register the callback");
	*/
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
      ::unlink("/tmp/etoile");

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

    // XXX

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
    /// XXX
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

      // reply with the Authenticated message.
      if (application->door->Reply(
	    elle::Inputs<TagAuthenticated>()) == elle::StatusError)
	escape("unable to reply to the application");

      leave();
    }

  }
}
