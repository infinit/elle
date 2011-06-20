//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/wall/Directory.cc
//
// created       julien quintard   [fri aug 14 16:34:43 2009]
// updated       julien quintard   [thu jun 16 11:08:40 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/wall/Directory.hh>

#include <etoile/gear/Identifier.hh>

#include <etoile/path/Path.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates an new, though orphan, directory object.
    ///
    elle::Status	Directory::Create(
			  gear::Identifier&			identifier)
    {
      /*
      context::Directory*	context;
      user::User*		user;

      enter(instance(context));

      printf("[XXX] Directory::Create()\n");

      // load the current user.
      if (user::User::Instance(user) == elle::StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // create a new context.
      if (context::Context::New(context) == elle::StatusError)
	escape("unable to allocate a new context");

      // create a new directory.
      if (components::Directory::Create(context) == elle::StatusError)
	escape("unable to create the directory");

      // return the context identifier to the caller.
      if (user->application->channel->Reply(
	    elle::Inputs<TagIdentifier>(context->identifier)) ==
	  elle::StatusError)
	escape("unable to reply to the application");

      // export the context.
      if (context::Context::Export(context) == elle::StatusError)
	escape("unable to export the context");

      // waive.
      waive(context);

      leave();
      */
    }

    ///
    /// this method loads the directory and creates a new context.
    ///
    elle::Status	Directory::Load(
			  const path::Way&			way,
			  gear::Identifier&			identifier)
    {
      /* XXX
	 instead of path::Way, we will receive a path::Chemin;

	 nucleus::Location	location;
	 gear::Scope*		scope;

	 chemin.Locate(location);

	 Gear::Select(identifier, &scope);

	 scope->automaton.XXX(location, ...);
       */

      /*
      context::Directory*	context;
      user::User*		user;

      enter(instance(context));

      printf("[XXX] Directory::Load()\n");

      // load the current user.
      if (user::User::Instance(user) == elle::StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // create a new context.
      if (context::Context::New(context) == elle::StatusError)
	escape("unable to allocate a new context");

      // create a route from the given way.
      if (context->route.Create(way) == elle::StatusError)
	escape("unable to create a route");

      // resolve the route in a directory address.
      if (path::Path::Resolve(context->route,
			      context->address) == elle::StatusError)
	escape("unable to resolve the given route into an directory address");

      // load the directory in the given context.
      if (components::Directory::Load(context,
				      context->address) == elle::StatusError)
	escape("unable to load the directory in the given context");

      // return the context identifier to the caller.
      if (user->application->channel->Reply(
	    elle::Inputs<TagIdentifier>(context->identifier)) ==
	  elle::StatusError)
	escape("unable to reply to the application");

      // export the context.
      if (context::Context::Export(context) == elle::StatusError)
	escape("unable to export the context");

      // waive.
      waive(context);

      leave();
      */
    }

    ///
    /// this application locks the directory the identified context is related
    /// to.
    ///
    /// the method returns true if the lock has been acquired or false
    /// otherwise.
    ///
    elle::Status	Directory::Lock(
			  const gear::Identifier&		identifier)
    {
      enter();

      printf("[XXX] Directory::Lock()\n");

      // XXX
      // XXX peut etre qu'il faudrait avoir une liste dependencies dans
      // un context qui signifierait que le context courant ne peut etre
      // commit tant que les dependences ne le sont pas!

      leave();
    }

    ///
    /// this method releases a previously locked directory.
    ///
    elle::Status	Directory::Release(
			  const gear::Identifier&		identifer)
    {
      enter();

      printf("[XXX] Directory::Release()\n");

      leave();
    }

    ///
    /// this method adds an entry.
    ///
    elle::Status	Directory::Add(
			  const gear::Identifier&		parent,
			  const path::Slice&			name,
			  const gear::Identifier&		child)
    {
      /*
      context::Directory*	directory;
      context::Directory*	subdirectory;
      user::User*		user;

      enter();

      printf("[XXX] Directory::Add()\n");

      // load the current user.
      if (user::User::Instance(user) == elle::StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // retrieve the directory context.
      if (user->application->Retrieve(parent, directory) == elle::StatusError)
	escape("unable to retrieve the context");

      // retrieve the subdirectory context.
      if (user->application->Retrieve(child, subdirectory) ==
	  elle::StatusError)
	escape("unable to retrieve the context");

      // request the directory component to add the entry.
      if (components::Directory::Add(directory,
				     name,
				     subdirectory) == elle::StatusError)
	escape("unable to add the entry");

      // reply to the caller.
      if (user->application->channel->Reply(
	    elle::Inputs<TagOk>()) == elle::StatusError)
	escape("unable to reply to the application");

      leave();
      */
    }

    ///
    /// this method returns the address associated with the given name.
    ///
    elle::Status	Directory::Lookup(
			  const gear::Identifier&		identifier,
			  const path::Slice&			name,
			  nucleus::Entry&			entry)
    {
      /*
      context::Directory*	context;
      user::User*		user;
      nucleus::Entry*		entry;

      enter();

      printf("[XXX] Directory::Lookup()\n");

      // load the current user.
      if (user::User::Instance(user) == elle::StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // retrieve the context.
      if (user->application->Retrieve(identifier, context) ==
	  elle::StatusError)
	escape("unable to retrieve the directory context");

      // check if the context is directory.
      if ((context->format & context::FormatDirectory) !=
	  context::FormatDirectory)
	escape("unable to test a non-directory object");

      // lookup the directory.
      if (components::Directory::Lookup(context,
					name,
					entry) == elle::StatusError)
	escape("unable to know if the given name is present in the directory");

      // return the status to the caller.
      if (entry == NULL)
	{
	  // return a null entry.
	  if (user->application->channel->Reply(
	        elle::Inputs<TagDirectoryEntry>(nucleus::Entry::Null)) ==
	      elle::StatusError)
	    escape("unable to reply to the application");
	}
      else
	{
	  // return the entry.
	  if (user->application->channel->Reply(
	        elle::Inputs<TagDirectoryEntry>(*entry)) == elle::StatusError)
	    escape("unable to reply to the application");
	}

      leave();
      */
    }

    ///
    /// this method returns a set [offset, offset + size[ of entries
    /// (name, address) from the directory identified by _identifier_.
    ///
    elle::Status	Directory::Consult(
			  const gear::Identifier&		identifier,
			  const nucleus::Offset&		offset,
			  const nucleus::Offset&		size,
			  nucleus::Range<nucleus::Entry>&	range)
    {
      /*
      context::Directory*		context;
      user::User*			user;
      nucleus::Range<nucleus::Entry>	range;

      enter();

      printf("[XXX] Directory::Consult()\n");

      // load the current user.
      if (user::User::Instance(user) == elle::StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // retrieve the context.
      if (user->application->Retrieve(identifier, context) ==
	  elle::StatusError)
	escape("unable to retrieve the directory context");

      // check if the context is directory.
      if ((context->format & context::FormatDirectory) !=
	  context::FormatDirectory)
	escape("unable to consult a non-directory object");

      // consult the directory.
      if (components::Directory::Consult(context,
					 offset,
					 size,
					 range) == elle::StatusError)
	escape("unable to consult the directory");

      // return the set to the caller.
      if (user->application->channel->Reply(
	    elle::Inputs<TagDirectoryRange>(range)) == elle::StatusError)
	escape("unable to reply to the application");

      leave();
      */
    }

    ///
    /// this method renames an entry of the given directory.
    ///
    elle::Status	Directory::Rename(
			  const gear::Identifier&		identifier,
			  const path::Slice&			from,
			  const path::Slice&			to)
    {
      /*
      context::Directory*	context;
      user::User*		user;

      enter();

      printf("[XXX] Directory::Rename()\n");

      // load the current user.
      if (user::User::Instance(user) == elle::StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // retrieve the context.
      if (user->application->Retrieve(identifier,
				      context) == elle::StatusError)
	escape("unable to retrieve the directory context");

      // check if the context is directory.
      if ((context->format & context::FormatDirectory) !=
	  context::FormatDirectory)
	escape("unable to rename a non-directory object");

      // rename the entry.
      if (components::Directory::Rename(context,
					from,
					to) == elle::StatusError)
	escape("unable to rename the entry");

      // return the set to the caller.
      if (user->application->channel->Reply(elle::Inputs<TagOk>()) ==
	  elle::StatusError)
	escape("unable to reply to the application");

      leave();
      */
    }

    ///
    /// this method removes an entry.
    ///
    elle::Status	Directory::Remove(
			  const gear::Identifier&		identifier,
			  const path::Slice&			name)
    {
      /*
      context::Directory*	context;
      user::User*		user;

      enter();

      printf("[XXX] Directory::Remove()\n");

      // load the current user.
      if (user::User::Instance(user) == elle::StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // retrieve the directory context.
      if (user->application->Retrieve(identifier, context) ==
	  elle::StatusError)
	escape("unable to retrieve the context");

      // request the directory component to remove the entry.
      if (components::Directory::Remove(context,
					name) == elle::StatusError)
	escape("unable to create the subdirectory");

      // reply to the caller.
      if (user->application->channel->Reply(
	    elle::Inputs<TagOk>()) == elle::StatusError)
	escape("unable to reply to the application");

      leave();
      */
    }

    ///
    /// this method discards the directory's modifications.
    ///
    elle::Status	Directory::Discard(
			  const gear::Identifier&		identifier)
    {
      /*
      context::Directory*	context;
      user::User*		user;

      enter();

      printf("[XXX] Directory::Discard()\n");

      // load the current user.
      if (user::User::Instance(user) == elle::StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // retrieve the context.
      if (user->application->Retrieve(identifier, context) ==
	  elle::StatusError)
	escape("unable to retrieve the directory context");

      // check if the context is directory.
      if ((context->format & context::FormatDirectory) !=
	  context::FormatDirectory)
	escape("unable to store a non-directory object");

      // discard the context.
      if (components::Directory::Discard(context) == elle::StatusError)
	escape("unable to discard the directory's modifications");

      // reply to the application.
      if (user->application->channel->Reply(elle::Inputs<TagOk>()) ==
	  elle::StatusError)
	escape("unable to reply to the application");

      leave();
      */
    }

    ///
    /// this method closes the context and applies, if required, the
    /// modifications.
    ///
    elle::Status	Directory::Store(
			  const gear::Identifier&		identifier)
    {
      /*
      context::Directory*	context;
      user::User*		user;

      enter();

      printf("[XXX] Directory::Store()\n");

      // load the current user.
      if (user::User::Instance(user) == elle::StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // retrieve the context.
      if (user->application->Retrieve(identifier, context) ==
	  elle::StatusError)
	escape("unable to retrieve the directory context");

      // check if the context is directory.
      if ((context->format & context::FormatDirectory) !=
	  context::FormatDirectory)
	escape("unable to store a non-directory object");

      // store the context.
      if (components::Directory::Store(context) == elle::StatusError)
	escape("unable to store the directory context");

      // reply to the application.
      if (user->application->channel->Reply(elle::Inputs<TagOk>()) ==
	  elle::StatusError)
	escape("unable to reply to the application");

      leave();
      */
    }

    ///
    /// this method destroys a directory.
    ///
    elle::Status	Directory::Destroy(
			  const gear::Identifier&		identifier)
    {
      /*
      context::Directory*	context;
      user::User*		user;

      enter();

      printf("[XXX] Directory::Destroy()\n");

      // load the current user.
      if (user::User::Instance(user) == elle::StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // retrieve the context.
      if (user->application->Retrieve(identifier,
				      context) == elle::StatusError)
	escape("unable to retrieve the directory context");

      // check if the context is directory.
      if ((context->format & context::FormatDirectory) !=
	  context::FormatDirectory)
	escape("unable to store a non-directory object");

      // destroy the context.
      if (components::Directory::Destroy(context) == elle::StatusError)
	escape("unable to destroy the directory context");

      // reply to the application.
      if (user->application->channel->Reply(elle::Inputs<TagOk>()) ==
	  elle::StatusError)
	escape("unable to reply to the application");

      leave();
      */
    }

  }
}
