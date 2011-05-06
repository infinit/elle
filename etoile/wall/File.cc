//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/wall/File.cc
//
// created       julien quintard   [fri aug 14 16:34:43 2009]
// updated       julien quintard   [fri may  6 14:01:09 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/wall/File.hh>

#include <etoile/context/Directory.hh>
#include <etoile/context/Format.hh>

#include <etoile/user/User.hh>

#include <etoile/path/Path.hh>

#include <etoile/Manifest.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates an new, though orphan, file object.
    ///
    elle::Status	File::Create()
    {
      context::File*	context;
      user::User*	user;

      enter(instance(context));

      printf("[XXX] File::Create()\n");

      // load the current user.
      if (user::User::Instance(user) == elle::StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // allocate a new context.
      if (context::Context::New(context) == elle::StatusError)
	escape("unable to allocate a new context");

      // create a new file.
      if (components::File::Create(context) == elle::StatusError)
	escape("unable to create the file");

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
    }

    ///
    /// this method loads the file and creates a new context.
    ///
    elle::Status	File::Load(const path::Way&		way)
    {
      context::File*	context;
      user::User*	user;

      enter(instance(context));

      printf("[XXX] File::Load()\n");

      // load the current user.
      if (user::User::Instance(user) == elle::StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // allocate a new context.
      if (context::Context::New(context) == elle::StatusError)
	escape("unable to allocate a new context");

      // create a route from the given way.
      if (context->route.Create(way) == elle::StatusError)
	escape("unable to create a route");

      // resolve the route in a file address.
      if (path::Path::Resolve(context->route, context->address) ==
	  elle::StatusError)
	escape("unable to resolve the given route into an file address");

      // load the file in the given context.
      if (components::File::Load(context,
				      context->address) == elle::StatusError)
	escape("unable to load the file in the given context");

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
    }

    ///
    /// this application locks the file the identified context is related
    /// to.
    ///
    /// the method returns true if the lock has been acquired or false
    /// otherwise.
    ///
    elle::Status	File::Lock(const
				     context::Identifier&	identifier)
    {
      enter();

      printf("[XXX] File::Lock()\n");

      // XXX
      // XXX peut etre qu'il faudrait avoir une liste dependencies dans
      // un context qui signifierait que le context courant ne peut etre
      // commit tant que les dependences ne le sont pas!

      leave();
    }

    ///
    /// this method releases a previously locked file.
    ///
    elle::Status	File::Release(const
				        context::Identifier&	identifer)
    {
      enter();

      printf("[XXX] File::Release()\n");

      leave();
    }

    ///
    /// this method writes a region of the file.
    ///
    elle::Status	File::Write(const context::Identifier& identifier,
				    const nucleus::Offset&	offset,
				    const elle::Region&		region)
    {
      context::File*	context;
      user::User*	user;

      enter();

      printf("[XXX] File::Write()\n");

      // load the current user.
      if (user::User::Instance(user) == elle::StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // retrieve the context.
      if (user->application->Retrieve(identifier, context) ==
	  elle::StatusError)
	escape("unable to retrieve the file context");

      // check if the context is file.
      if ((context->format & context::FormatFile) !=
	  context::FormatFile)
	escape("unable to test a non-file object");

      // write the file.
      if (components::File::Write(context, offset, region) ==
	  elle::StatusError)
	escape("unable to write the file");

      // answer the caller.
      if (user->application->channel->Reply(
	    elle::Inputs<TagOk>()) == elle::StatusError)
	escape("unable to reply to the application");

      leave();
    }

    ///
    /// this method reads a region of the file.
    ///
    elle::Status	File::Read(const context::Identifier&	identifier,
				   const nucleus::Offset&	offset,
				   const nucleus::Size&		size)
    {
      context::File*	context;
      user::User*	user;
      elle::Region	region;

      enter();

      printf("[XXX] File::Read()\n");

      // load the current user.
      if (user::User::Instance(user) == elle::StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // retrieve the file context.
      if (user->application->Retrieve(identifier, context) ==
	  elle::StatusError)
	escape("unable to retrieve the context");

      // request the file component.
      if (components::File::Read(context, offset, size, region) ==
	  elle::StatusError)
	escape("unable to read the file");

      // reply to the caller.
      if (user->application->channel->Reply(
	    elle::Inputs<TagFileRegion>(region)) == elle::StatusError)
	escape("unable to reply to the application");

      leave();
    }

    ///
    /// this method adjusts the size of a file.
    ///
    elle::Status	File::Adjust(const context::Identifier&	identifier,
				     const nucleus::Size&	size)
    {
      context::File*	context;
      user::User*	user;

      enter();

      printf("[XXX] File::Adjust()\n");

      // load the current user.
      if (user::User::Instance(user) == elle::StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // retrieve the context.
      if (user->application->Retrieve(identifier, context) ==
	  elle::StatusError)
	escape("unable to retrieve the file context");

      // check if the context is file.
      if ((context->format & context::FormatFile) !=
	  context::FormatFile)
	escape("unable to rename a non-file object");

      // adjust the file.
      if (components::File::Adjust(context, size) == elle::StatusError)
	escape("unable to adjust the file size");

      // return the set to the caller.
      if (user->application->channel->Reply(elle::Inputs<TagOk>()) ==
	  elle::StatusError)
	escape("unable to reply to the application");

      leave();
    }

    ///
    /// this method discards the file's modifications.
    ///
    elle::Status	File::Discard(const context::Identifier& identifier)
    {
      context::File*	context;
      user::User*	user;

      enter();

      printf("[XXX] File::Discard()\n");

      // load the current user.
      if (user::User::Instance(user) == elle::StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // retrieve the context.
      if (user->application->Retrieve(identifier, context) ==
	  elle::StatusError)
	escape("unable to retrieve the file context");

      // check if the context is file.
      if ((context->format & context::FormatFile) !=
	  context::FormatFile)
	escape("unable to store a non-file object");

      // discard the context.
      if (components::File::Discard(context) == elle::StatusError)
	escape("unable to discard the file's modifications");

      // reply to the application.
      if (user->application->channel->Reply(elle::Inputs<TagOk>()) ==
	  elle::StatusError)
	escape("unable to reply to the application");

      leave();
    }

    ///
    /// this method closes the context and applies, if required, the
    /// modifications.
    ///
    elle::Status	File::Store(const context::Identifier&	identifier)
    {
      context::File*	context;
      user::User*	user;

      enter();

      printf("[XXX] File::Store()\n");

      // load the current user.
      if (user::User::Instance(user) == elle::StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // retrieve the context.
      if (user->application->Retrieve(identifier, context) ==
	  elle::StatusError)
	escape("unable to retrieve the file context");

      // check if the context is file.
      if ((context->format & context::FormatFile) !=
	  context::FormatFile)
	escape("unable to store a non-file object");

      // store the context.
      if (components::File::Store(context) == elle::StatusError)
	escape("unable to store the file context");

      // reply to the application.
      if (user->application->channel->Reply(elle::Inputs<TagOk>()) ==
	  elle::StatusError)
	escape("unable to reply to the application");

      leave();
    }

    ///
    /// this method destroys a file.
    ///
    elle::Status	File::Destroy(const context::Identifier& identifier)
    {
      context::File*	context;
      user::User*	user;

      enter();

      printf("[XXX] File::Destroy()\n");

      // load the current user.
      if (user::User::Instance(user) == elle::StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // retrieve the context.
      if (user->application->Retrieve(identifier, context) ==
	  elle::StatusError)
	escape("unable to retrieve the file context");

      // check if the context is file.
      if ((context->format & context::FormatFile) !=
	  context::FormatFile)
	escape("unable to store a non-file object");

      // destroy the context.
      if (components::File::Destroy(context) == elle::StatusError)
	escape("unable to destroy the file context");

      // reply to the application.
      if (user->application->channel->Reply(elle::Inputs<TagOk>()) ==
	  elle::StatusError)
	escape("unable to reply to the application");

      leave();
    }

  }
}
