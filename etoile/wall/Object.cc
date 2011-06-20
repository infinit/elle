//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/wall/Object.cc
//
// created       julien quintard   [wed mar  3 20:50:57 2010]
// updated       julien quintard   [tue jun 14 14:16:31 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/wall/Object.hh>

#include <etoile/path/Path.hh>

#include <etoile/miscellaneous/Information.hh>

// XXX #include <etoile/automaton/Object.hh>
// #include <etoile/context/Object.hh>
// #include <etoile/context/Format.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method loads an object in a context and returns the context
    /// identifier.
    ///
    elle::Status	Object::Load(
			  const path::Way&			way,
			  gear::Identifier&			identifier)
    {
      /* XXX
      context::Object*		context;

      enter(instance(context));

      // create a new context.
      if (context::Context::New(context) == elle::StatusError)
	escape("unable to allocate a new context");

      // create a route from the given way.
      if (context->route.Create(way) == elle::StatusError)
	escape("unable to create a route");

      // resolve the route in an object address.
      if (path::Path::Resolve(context->route,
			      context->address) == elle::StatusError)
	escape("unable to resolve the given route into an object's address");

      // load the object in the given context.
      if (components::Object::Load(context,
				   context->address) == elle::StatusError)
	escape("unable to load the object in the given context");

      // return the context identifier to the caller.
      identifier = context->identifier;

      // export the context.
      if (context::Context::Export(context) == elle::StatusError)
	escape("unable to export the context");

      // waive to tracking.
      waive(context);

      leave();
      */
    }

    ///
    /// this method locks the object the identified context is related
    /// to.
    ///
    /// the method returns true if the lock has been acquired or false
    /// otherwise.
    ///
    elle::Status	Object::Lock(
			  const gear::Identifier&		identifier)
    {
      enter();

      printf("[XXX] Object::Lock()\n");

      leave();
    }

    ///
    /// this method releases a previously locked object.
    ///
    elle::Status	Object::Release(
			  const gear::Identifier&		identifer)
    {
      enter();

      printf("[XXX] Object::Release()\n");

      leave();
    }

    ///
    /// this method returns information on the object in a compact format.
    ///
    elle::Status	Object::Information(
			  const gear::Identifier&		identifier,
			  miscellaneous::Information&		information)
    {
      /* XXX
      context::Object*		context;
      Status			status;
      user::User*		user;

      enter();

      printf("[XXX] Object::Information()\n");

      // load the current user.
      if (user::User::Instance(user) == elle::StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // retrieve the context.
      if (user->application->Retrieve(identifier, context) ==
	  elle::StatusError)
	escape("unable to retrieve the object context");

      // check if the context is an object.
      if ((context->format & context::FormatObject) !=
	  context::FormatObject)
	escape("unable to get information on non-object contexts");

      // request the object component to fill the status structure.
      if (components::Object::Information(context,
					  status) == elle::StatusError)
	escape("unable to retrieve information on the object");

      // return the status to the caller.
      if (user->application->channel->Reply(
	    elle::Inputs<TagObjectStatus>(status)) == elle::StatusError)
	escape("unable to reply to the application");

      leave();
      */
    }

    ///
    /// this method discards the modifications.
    ///
    elle::Status	Object::Discard(
			  const gear::Identifier&		identifier)
    {
      /* XXX
      context::Object*		context;
      user::User*		user;

      enter();

      printf("[XXX] Object::Discard()\n");

      // load the current user.
      if (user::User::Instance(user) == elle::StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // retrieve the context.
      if (user->application->Retrieve(identifier, context) ==
	  elle::StatusError)
	escape("unable to retrieve the object context");

      // check if the context is exactly an object.
      if ((context->format & context::FormatObject) !=
	  context::FormatObject)
	escape("unable to discard non-object contexts");

      // discard the context.
      if (components::Object::Discard(context) == elle::StatusError)
	escape("unable to discard the object's modifications");

      // reply to the application.
      if (user->application->channel->Reply(elle::Inputs<TagOk>()) ==
	  elle::StatusError)
	escape("unable to reply to the application");

      leave();
      */
    }

    ///
    /// this method commits the modifications pending on the context
    /// and closes it.
    ///
    elle::Status	Object::Store(
			  const gear::Identifier&		identifier)
    {
      /*
      context::Object*		context;
      user::User*		user;

      enter();

      printf("[XXX] Object::Store()\n");

      // load the current user.
      if (user::User::Instance(user) == elle::StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // retrieve the context.
      if (user->application->Retrieve(identifier, context) ==
	  elle::StatusError)
	escape("unable to retrieve the object context");

      // check if the context is exactly an object.
      if (context->format != context::FormatObject)
	escape("unable to store non-object contexts");

      // store the context.
      if (components::Object::Store(context) == elle::StatusError)
	escape("unable to store the object context");
      /* XXX
      // reply to the application.
      if (user->application->channel->Reply(elle::Inputs<TagOk>()) ==
	  elle::StatusError)
	escape("unable to reply to the application");

      leave();
      */
    }

  }
}
