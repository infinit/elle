//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/wall/Link.cc
//
// created       julien quintard   [fri aug 14 16:34:43 2009]
// updated       julien quintard   [wed jun  1 12:02:55 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/wall/Link.hh>

#include <etoile/context/Link.hh>
#include <etoile/context/Format.hh>

#include <etoile/user/User.hh>

#include <etoile/path/Path.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates an new, though orphan, link object.
    ///
    elle::Status	Link::Create()
    {
      context::Link*	context;
      user::User*	user;

      enter(instance(context));

      printf("[XXX] Link::Create()\n");

      // load the current user.
      if (user::User::Instance(user) == elle::StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // allocate a new context.
      if (context::Context::New(context) == elle::StatusError)
	escape("unable to allocate a new context");

      // create a new link.
      if (components::Link::Create(context) == elle::StatusError)
	escape("unable to create the link");

      /* XXX
      // return the context identifier to the caller.
      if (user->application->channel->Reply(
	    elle::Inputs<TagIdentifier>(context->identifier)) ==
	  elle::StatusError)
	escape("unable to reply to the application");
      */

      // export the context.
      if (context::Context::Export(context) == elle::StatusError)
	escape("unable to export the context");

      // waive.
      waive(context);

      leave();
    }

    ///
    /// this method loads the link and creates a new context.
    ///
    elle::Status	Link::Load(const path::Way&		way)
    {
      context::Link*	context;
      user::User*	user;

      enter(instance(context));

      printf("[XXX] Link::Load()\n");

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

      // resolve the route in a link address.
      if (path::Path::Resolve(context->route, context->address) ==
	  elle::StatusError)
	escape("unable to resolve the given route into an link address");

      // load the link in the given context.
      if (components::Link::Load(context,
				 context->address) == elle::StatusError)
	escape("unable to load the link in the given context");

      /* XXX
      // return the context identifier to the caller.
      if (user->application->channel->Reply(
	    elle::Inputs<TagIdentifier>(context->identifier)) ==
	  elle::StatusError)
	escape("unable to reply to the application");
      */

      // export the context.
      if (context::Context::Export(context) == elle::StatusError)
	escape("unable to export the context");

      // waive.
      waive(context);

      leave();
    }

    ///
    /// this application locks the link the identified context is related
    /// to.
    ///
    /// the method returns true if the lock has been acquired or false
    /// otherwise.
    ///
    elle::Status	Link::Lock(const context::Identifier&	identifier)
    {
      enter();

      printf("[XXX] Link::Lock()\n");

      // XXX
      // XXX peut etre qu'il faudrait avoir une liste dependencies dans
      // un context qui signifierait que le context courant ne peut etre
      // commit tant que les dependences ne le sont pas!

      leave();
    }

    ///
    /// this method releases a previously locked link.
    ///
    elle::Status	Link::Release(const context::Identifier& identifer)
    {
      enter();

      printf("[XXX] Link::Release()\n");

      leave();
    }

    ///
    /// this method binds a new target way to the given object.
    ///
    elle::Status	Link::Bind(const context::Identifier&	identifier,
				   const path::Way&		way)
    {
      context::Link*	context;
      user::User*	user;

      enter();

      printf("[XXX] Link::Bind()\n");

      // load the current user.
      if (user::User::Instance(user) == elle::StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // retrieve the context.
      if (user->application->Retrieve(identifier, context) ==
	  elle::StatusError)
	escape("unable to retrieve the link context");

      // check if the context is link.
      if ((context->format & context::FormatLink) !=
	  context::FormatLink)
	escape("unable to test a non-link object");

      // bind the link.
      if (components::Link::Bind(context, way) == elle::StatusError)
	escape("unable to bind the target to the link");

      /* XXX
      // answer the caller.
      if (user->application->channel->Reply(
	    elle::Inputs<TagOk>()) == elle::StatusError)
	escape("unable to reply to the application");
      */

      leave();
    }

    ///
    /// this method returns the way pointed by this link.
    ///
    elle::Status	Link::Resolve(const context::Identifier& identifier)
    {
      context::Link*	context;
      user::User*	user;
      path::Way		way;

      enter();

      printf("[XXX] Link::Resolve()\n");

      // load the current user.
      if (user::User::Instance(user) == elle::StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // retrieve the context.
      if (user->application->Retrieve(identifier, context) ==
	  elle::StatusError)
	escape("unable to retrieve the link context");

      // check if the context is link.
      if ((context->format & context::FormatLink) !=
	  context::FormatLink)
	escape("unable to consult a non-link object");

      // request the components.
      if (components::Link::Resolve(context, way) == elle::StatusError)
	escape("unable to consult the link");

      /* XXX
      // return the way to the caller.
      if (user->application->channel->Reply(
	    elle::Inputs<TagLinkWay>(way)) == elle::StatusError)
	escape("unable to reply to the application");
      */

      leave();
    }

    ///
    /// this method discards the link's modifications.
    ///
    elle::Status	Link::Discard(const context::Identifier& identifier)
    {
      context::Link*	context;
      user::User*	user;

      enter();

      printf("[XXX] Link::Discard()\n");

      // load the current user.
      if (user::User::Instance(user) == elle::StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // retrieve the context.
      if (user->application->Retrieve(identifier, context) ==
	  elle::StatusError)
	escape("unable to retrieve the link context");

      // check if the context is link.
      if ((context->format & context::FormatLink) !=
	  context::FormatLink)
	escape("unable to store a non-link object");

      // discard the context.
      if (components::Link::Discard(context) == elle::StatusError)
	escape("unable to discard the link's modifications");

      /* XXX
      // reply to the application.
      if (user->application->channel->Reply(elle::Inputs<TagOk>()) ==
	  elle::StatusError)
	escape("unable to reply to the application");
      */

      leave();
    }

    ///
    /// this method closes the context and applies, if required, the
    /// modifications.
    ///
    elle::Status	Link::Store(const context::Identifier&	identifier)
    {
      context::Link*	context;
      user::User*	user;

      enter();

      printf("[XXX] Link::Store()\n");

      // load the current user.
      if (user::User::Instance(user) == elle::StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // retrieve the context.
      if (user->application->Retrieve(identifier, context) ==
	  elle::StatusError)
	escape("unable to retrieve the link context");

      // check if the context is link.
      if ((context->format & context::FormatLink) !=
	  context::FormatLink)
	escape("unable to store a non-link object");

      // store the context.
      if (components::Link::Store(context) == elle::StatusError)
	escape("unable to store the link context");

      /* XXX
      // reply to the application.
      if (user->application->channel->Reply(elle::Inputs<TagOk>()) ==
	  elle::StatusError)
	escape("unable to reply to the application");
      */

      leave();
    }

    ///
    /// this method destroys a link.
    ///
    elle::Status	Link::Destroy(const context::Identifier& identifier)
    {
      context::Link*	context;
      user::User*	user;

      enter();

      printf("[XXX] Link::Destroy()\n");

      // load the current user.
      if (user::User::Instance(user) == elle::StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // retrieve the context.
      if (user->application->Retrieve(identifier, context) ==
	  elle::StatusError)
	escape("unable to retrieve the link context");

      // check if the context is link.
      if ((context->format & context::FormatLink) !=
	  context::FormatLink)
	escape("unable to store a non-link object");

      // destroy the context.
      if (components::Link::Destroy(context) == elle::StatusError)
	escape("unable to destroy the link context");

      /* XXX
      // reply to the application.
      if (user->application->channel->Reply(elle::Inputs<TagOk>()) ==
	  elle::StatusError)
	escape("unable to reply to the application");
      */

      leave();
    }

  }
}
