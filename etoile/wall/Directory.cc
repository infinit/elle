//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/wall/Directory.cc
//
// created       julien quintard   [fri aug 14 16:34:43 2009]
// updated       julien quintard   [wed apr  7 20:46:12 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/wall/Directory.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method loads the directory and creates a new context.
    ///
    Status		Directory::Load(const
					  path::Way&		way)
    {
      context::Directory*	context;
      context::Identifier	identifier;
      user::User*		user;

      enter(instance(context));

      printf("[XXX] Directory::Load()\n");

      // load the current user.
      if (user::User::Instance(user) == StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // allocate a new context.
      context = new context::Directory;

      // create a route from the given way.
      if (context->route.Create(way) == StatusError)
	escape("unable to create a route");

      // resolve the route in a directory address.
      if (path::Path::Resolve(context->route, context->address) == StatusError)
	escape("unable to resolve the given route into an directory address");

      // load the directory in the given context.
      if (components::Directory::Load(context,
				      context->address) == StatusError)
	escape("unable to load the directory in the given context");

      // generate an identifier.
      if (identifier.Generate() == StatusError)
	escape("unable to generate an identifier");

      // store the context in the container.
      if (context::Context::Add(identifier, context) == StatusError)
	escape("unable to store the context");

      // waive the context.
      waive(context);

      // return the context identifier to the caller.
      if (user->application->channel->Reply(
	    Inputs<TagIdentifier>(identifier)) == StatusError)
	escape("unable to reply to the application");

      leave();
    }

    ///
    /// this method creates a subdirector in the given directory.
    ///
    Status		Directory::Create(const
					    path::Way&		way)
    {
      path::Slice		name;
      path::Way			path(way, name);
      context::Directory*	directory;
      context::Directory*	subdirectory;
      context::Identifier	identifier;
      user::User*		user;

      enter(instance(directory),
	    instance(subdirectory));

      printf("[XXX] Directory::Create()\n");

      // load the current user.
      if (user::User::Instance(user) == StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // allocate a new context.
      directory = new context::Directory;

      // create a route from the path way.
      if (directory->route.Create(path) == StatusError)
	escape("unable to create a route");

      // resolve the route in a directory address.
      if (path::Path::Resolve(directory->route,
			      directory->address) == StatusError)
	escape("unable to resolve the given route into an directory address");

      // load the parent directory in the context.
      if (components::Directory::Load(directory,
				      directory->address) == StatusError)
	escape("unable to load the directory in the given context");

      // allocate the subdirectory context.
      subdirectory = new context::Directory;

      // request the directory component to create the subdirectory.
      if (components::Directory::Create(directory,
					name,
					subdirectory) == StatusError)
	escape("unable to create the subdirectory");

      // store the parent directory.
      if (components::Directory::Store(directory) == StatusError)
	escape("unable to store the parent directory");

      // waive the parent context.
      waive(directory);

      // generate an identifier.
      if (identifier.Generate() == StatusError)
	escape("unable to generate an identifier");

      // store the context in the container.
      if (context::Context::Add(identifier, subdirectory) == StatusError)
	escape("unable to store the context");

      // waive the child context.
      waive(subdirectory);

      // return the context identifier to the caller.
      if (user->application->channel->Reply(
	    Inputs<TagIdentifier>(identifier)) == StatusError)
	escape("unable to reply to the application");

      leave();
    }

    ///
    /// this application locks the directory the identified context is related
    /// to.
    ///
    /// the method returns true if the lock has been acquired or false
    /// otherwise.
    ///
    Status		Directory::Lock(const
					  context::Identifier&	identifier)
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
    Status		Directory::Release(const
					     context::Identifier& identifer)
    {
      enter();

      printf("[XXX] Directory::Release()\n");

      leave();
    }

    ///
    /// this method tests if the given name is present in the directory
    /// entries.
    ///
    /// therefore this method returns true or false to the caller.
    ///
    Status		Directory::Exist(const
					   context::Identifier& identifier,
					 const
					   path::Slice&		name)
    {
      context::Directory*	context;
      user::User*		user;
      Boolean			boolean;

      enter();

      printf("[XXX] Directory::Exist()\n");

      // load the current user.
      if (user::User::Instance(user) == StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // retrieve the context.
      if (context::Context::Retrieve(identifier, context) == StatusError)
	escape("unable to retrieve the directory context");

      // check if the context is directory.
      if ((context->format & context::FormatDirectory) !=
	  context::FormatDirectory)
	escape("unable to test a non-directory object");

      // test the directory.
      if (components::Directory::Exist(context,
				       name,
				       boolean) == StatusError)
	escape("unable to know if the given name is present in the directory");

      // return the status to the caller.
      if (user->application->channel->Reply(
	    Inputs<TagBoolean>(boolean)) == StatusError)
	escape("unable to reply to the application");

      leave();
    }

    ///
    /// this method returns the address associated with the given name.
    ///
    Status		Directory::Lookup(const
					    context::Identifier& identifier,
					  const
					    path::Slice&	name)
    {
      context::Directory*	context;
      user::User*		user;
      kernel::Entry*		entry;

      enter();

      printf("[XXX] Directory::Lookup()\n");

      // load the current user.
      if (user::User::Instance(user) == StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // retrieve the context.
      if (context::Context::Retrieve(identifier, context) == StatusError)
	escape("unable to retrieve the directory context");

      // check if the context is directory.
      if ((context->format & context::FormatDirectory) !=
	  context::FormatDirectory)
	escape("unable to test a non-directory object");

      // lookup the directory.
      if (components::Directory::Lookup(context,
					name,
					entry) == StatusError)
	escape("unable to know if the given name is present in the directory");

      // return the status to the caller.
      if (user->application->channel->Reply(
	    Inputs<TagDirectoryEntry>(*entry)) == StatusError)
	escape("unable to reply to the application");

      leave();
    }

    ///
    /// this method returns a set [offset, offset + size[ of entries
    /// (name, address) from the directory identified by _identifier_.
    ///
    Status		Directory::Consult(const
					     context::Identifier& identifier,
					   const
					     kernel::Offset&	offset,
					   const
					     kernel::Offset&	size)
    {
      context::Directory*	context;
      user::User*		user;
      kernel::Set		set;

      enter();

      printf("[XXX] Directory::Consult()\n");

      // load the current user.
      if (user::User::Instance(user) == StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // retrieve the context.
      if (context::Context::Retrieve(identifier, context) == StatusError)
	escape("unable to retrieve the directory context");

      // check if the context is directory.
      if ((context->format & context::FormatDirectory) !=
	  context::FormatDirectory)
	escape("unable to consult a non-directory object");

      // consult the directory.
      if (components::Directory::Consult(context,
					 offset,
					 size,
					 set) == StatusError)
	escape("unable to consult the directory");

      // return the set to the caller.
      if (user->application->channel->Reply(
	    Inputs<TagDirectorySet>(set)) == StatusError)
	escape("unable to reply to the application");

      leave();
    }

    ///
    /// this method renames an entry of the given directory.
    ///
    Status		Directory::Rename(const
					    context::Identifier& identifier,
					  const
					    path::Slice&	from,
					  const
					    path::Slice&	to)
    {
      context::Directory*	context;
      user::User*		user;

      enter();

      printf("[XXX] Directory::Rename()\n");

      // load the current user.
      if (user::User::Instance(user) == StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // retrieve the context.
      if (context::Context::Retrieve(identifier, context) == StatusError)
	escape("unable to retrieve the directory context");

      // check if the context is directory.
      if ((context->format & context::FormatDirectory) !=
	  context::FormatDirectory)
	escape("unable to rename a non-directory object");

      // rename the entry.
      if (components::Directory::Rename(context,
					from,
					to) == StatusError)
	escape("unable to rename the entry");

      // return the set to the caller.
      if (user->application->channel->Reply(Inputs<TagOk>()) == StatusError)
	escape("unable to reply to the application");

      leave();
    }

    ///
    /// this method closes the context and applies, if required, the
    /// modifications.
    ///
    Status		Directory::Store(const
					   context::Identifier&	identifier)
    {
      context::Directory*	context;
      user::User*		user;

      enter();

      printf("[XXX] Directory::Store()\n");

      // load the current user.
      if (user::User::Instance(user) == StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // retrieve the context.
      if (context::Context::Retrieve(identifier, context) == StatusError)
	escape("unable to retrieve the directory context");

      // check if the context is directory.
      if ((context->format & context::FormatDirectory) !=
	  context::FormatDirectory)
	escape("unable to store a non-directory object");

      // store the context.
      if (components::Directory::Store(context) == StatusError)
	escape("unable to store the directory context");

      // reply to the application.
      if (user->application->channel->Reply(Inputs<TagOk>()) == StatusError)
	escape("unable to reply to the application");

      leave();
    }

  }
}
