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
// updated       julien quintard   [thu apr 22 11:09:07 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/wall/Link.hh>

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
    Status		Link::Create()
    {
      context::Link*	context;
      user::User*	user;

      enter(instance(context));

      printf("[XXX] Link::Create()\n");

      // load the current user.
      if (user::User::Instance(user) == StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // allocate a new context.
      if (context::Context::New(context) == StatusError)
	escape("unable to allocate a new context");

      // create a new link.
      if (components::Link::Create(context) == StatusError)
	escape("unable to create the link");

      // return the context identifier to the caller.
      if (user->application->channel->Reply(
	    Inputs<TagIdentifier>(context->identifier)) == StatusError)
	escape("unable to reply to the application");

      // export the context.
      if (context::Context::Export(context) == StatusError)
	escape("unable to export the context");

      // waive.
      waive(context);

      leave();
    }

    ///
    /// this method loads the link and creates a new context.
    ///
    Status		Link::Load(const path::Way&		way)
    {
      context::Link*	context;
      user::User*	user;

      enter(instance(context));

      printf("[XXX] Link::Load()\n");

      // load the current user.
      if (user::User::Instance(user) == StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // allocate a new context.
      if (context::Context::New(context) == StatusError)
	escape("unable to allocate a new context");

      // create a route from the given way.
      if (context->route.Create(way) == StatusError)
	escape("unable to create a route");

      // resolve the route in a link address.
      if (path::Path::Resolve(context->route, context->address) == StatusError)
	escape("unable to resolve the given route into an link address");

      // load the link in the given context.
      if (components::Link::Load(context,
				 context->address) == StatusError)
	escape("unable to load the link in the given context");

      // return the context identifier to the caller.
      if (user->application->channel->Reply(
	    Inputs<TagIdentifier>(context->identifier)) == StatusError)
	escape("unable to reply to the application");

      // export the context.
      if (context::Context::Export(context) == StatusError)
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
    Status		Link::Lock(const context::Identifier&	identifier)
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
    Status		Link::Release(const context::Identifier& identifer)
    {
      enter();

      printf("[XXX] Link::Release()\n");

      leave();
    }

    ///
    /// this method binds a new target way to the given object.
    ///
    Status		Link::Bind(const context::Identifier&	identifier,
				   const path::Way&		way)
    {
      context::Link*	context;
      user::User*	user;

      enter();

      printf("[XXX] Link::Bind()\n");

      // load the current user.
      if (user::User::Instance(user) == StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // retrieve the context.
      if (user->application->Retrieve(identifier, context) == StatusError)
	escape("unable to retrieve the link context");

      // check if the context is link.
      if ((context->format & context::FormatLink) !=
	  context::FormatLink)
	escape("unable to test a non-link object");

      // bind the link.
      if (components::Link::Bind(context, way) == StatusError)
	escape("unable to bind the target to the link");

      // answer the caller.
      if (user->application->channel->Reply(
	    Inputs<TagOk>()) == StatusError)
	escape("unable to reply to the application");

      leave();
    }

    ///
    /// this method returns the way pointed by this link.
    ///
    Status		Link::Resolve(const context::Identifier& identifier)
    {
      context::Link*	context;
      user::User*	user;
      path::Way		way;

      enter();

      printf("[XXX] Link::Resolve()\n");

      // load the current user.
      if (user::User::Instance(user) == StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // retrieve the context.
      if (user->application->Retrieve(identifier, context) == StatusError)
	escape("unable to retrieve the link context");

      // check if the context is link.
      if ((context->format & context::FormatLink) !=
	  context::FormatLink)
	escape("unable to consult a non-link object");

      // request the components.
      if (components::Link::Resolve(context, way) == StatusError)
	escape("unable to consult the link");

      // return the way to the caller.
      if (user->application->channel->Reply(
	    Inputs<TagLinkWay>(way)) == StatusError)
	escape("unable to reply to the application");

      leave();
    }

    ///
    /// this method discards the link's modifications.
    ///
    Status		Link::Discard(const context::Identifier& identifier)
    {
      context::Link*	context;
      user::User*	user;

      enter();

      printf("[XXX] Link::Discard()\n");

      // load the current user.
      if (user::User::Instance(user) == StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // retrieve the context.
      if (user->application->Retrieve(identifier, context) == StatusError)
	escape("unable to retrieve the link context");

      // check if the context is link.
      if ((context->format & context::FormatLink) !=
	  context::FormatLink)
	escape("unable to store a non-link object");

      // discard the context.
      if (components::Link::Discard(context) == StatusError)
	escape("unable to discard the link's modifications");

      // reply to the application.
      if (user->application->channel->Reply(Inputs<TagOk>()) == StatusError)
	escape("unable to reply to the application");

      leave();
    }

    ///
    /// this method closes the context and applies, if required, the
    /// modifications.
    ///
    Status		Link::Store(const context::Identifier&	identifier)
    {
      context::Link*	context;
      user::User*	user;

      enter();

      printf("[XXX] Link::Store()\n");

      // load the current user.
      if (user::User::Instance(user) == StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // retrieve the context.
      if (user->application->Retrieve(identifier, context) == StatusError)
	escape("unable to retrieve the link context");

      // check if the context is link.
      if ((context->format & context::FormatLink) !=
	  context::FormatLink)
	escape("unable to store a non-link object");

      // store the context.
      if (components::Link::Store(context) == StatusError)
	escape("unable to store the link context");

      // reply to the application.
      if (user->application->channel->Reply(Inputs<TagOk>()) == StatusError)
	escape("unable to reply to the application");

      leave();
    }

    ///
    /// this method destroys a link.
    ///
    Status		Link::Destroy(const context::Identifier& identifier)
    {
      context::Link*	context;
      user::User*	user;

      enter();

      printf("[XXX] Link::Destroy()\n");

      // load the current user.
      if (user::User::Instance(user) == StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // retrieve the context.
      if (user->application->Retrieve(identifier, context) == StatusError)
	escape("unable to retrieve the link context");

      // check if the context is link.
      if ((context->format & context::FormatLink) !=
	  context::FormatLink)
	escape("unable to store a non-link object");

      // destroy the context.
      if (components::Link::Destroy(context) == StatusError)
	escape("unable to destroy the link context");

      // reply to the application.
      if (user->application->channel->Reply(Inputs<TagOk>()) == StatusError)
	escape("unable to reply to the application");

      leave();
    }

  }
}
