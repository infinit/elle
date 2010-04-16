//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/wall/Attributes.cc
//
// created       julien quintard   [wed mar 31 19:26:06 2010]
// updated       julien quintard   [thu apr 15 19:08:38 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/wall/Attributes.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method adds a trait to the object's attributes.
    ///
    Status		Attributes::Add(const
				          context::Identifier&	identifier,
					const String&		name,
					const String&		value)
    {
      context::Object*	context;
      user::User*	user;

      enter();

      printf("[XXX] Attributes::Add()\n");

      // load the current user.
      if (user::User::Instance(user) == StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // retrieve the context.
      if (context::Context::Retrieve(identifier, context) == StatusError)
	escape("unable to retrieve the object context");

      // check if the context is an object.
      if ((context->format & context::FormatObject) !=
	  context::FormatObject)
	escape("unable to test non-objects");

      // request the components.
      if (components::Attributes::Add(context,
				      name,
				      value) == StatusError)
	escape("unable to add the attribute");

      // answer the caller.
      if (user->application->channel->Reply(Inputs<TagOk>()) == StatusError)
	escape("unable to reply to the application");

      leave();
    }

    ///
    /// this method returns the caller the trait associated with
    /// the given name.
    ///
    Status		Attributes::Lookup(const
					     context::Identifier& identifier,
					   const String&	name)
    {
      context::Object*	context;
      user::User*	user;
      kernel::Trait*	trait;

      enter();

      printf("[XXX] Attributes::Lookup()\n");

      // load the current user.
      if (user::User::Instance(user) == StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // retrieve the context.
      if (context::Context::Retrieve(identifier, context) == StatusError)
	escape("unable to retrieve the object context");

      // check if the context is an object.
      if ((context->format & context::FormatObject) !=
	  context::FormatObject)
	escape("unable to test non-objects");

      // request the components.
      if (components::Attributes::Lookup(context, name, trait) == StatusError)
	escape("unable to retrieve the trait");

      // answer the caller, depending on the result.
      if (trait == NULL)
	{
	  // return the null trait.
	  if (user->application->channel->Reply(
		Inputs<TagAttributesTrait>(kernel::Trait::Null)) ==
	      StatusError)
	    escape("unable to reply to the application");
	}
      else
	{
	  // return the trait.
	  if (user->application->channel->Reply(
	        Inputs<TagAttributesTrait>(*trait)) == StatusError)
	    escape("unable to reply to the application");
	}

      leave();
    }

    ///
    /// this method returns a subset of the attributes list.
    ///
    Status		Attributes::Consult(const
					      context::Identifier& identifier,
					    const
					      kernel::Index&	index,
					    const
					      kernel::Size&	size)
    {
      context::Object*		context;
      user::User*		user;
      kernel::Collection	collection;

      enter();

      printf("[XXX] Attributes::Consult()\n");

      // load the current user.
      if (user::User::Instance(user) == StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // retrieve the context.
      if (context::Context::Retrieve(identifier, context) == StatusError)
	escape("unable to retrieve the object context");

      // check if the context is an object.
      if ((context->format & context::FormatObject) !=
	  context::FormatObject)
	escape("unable to test non-objects");

      // request the components.
      if (components::Attributes::Consult(context,
					  index,
					  size,
					  collection) == StatusError)
	escape("unable to consult the attributes list");

      // answer the caller.
      if (user->application->channel->Reply(
	    Inputs<TagAttributesCollection>(collection)) == StatusError)
	escape("unable to reply to the application");

      leave();
    }

    ///
    /// this method updates an existing trait with a new value.
    ///
    Status		Attributes::Update(const
					     context::Identifier& identifier,
					   const String&	name,
					   const String&	value)
    {
      context::Object*	context;
      user::User*	user;

      enter();

      printf("[XXX] Attributes::Update()\n");

      // load the current user.
      if (user::User::Instance(user) == StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // retrieve the context.
      if (context::Context::Retrieve(identifier, context) == StatusError)
	escape("unable to retrieve the object context");

      // check if the context is an object.
      if ((context->format & context::FormatObject) !=
	  context::FormatObject)
	escape("unable to test non-objects");

      // request the components.
      if (components::Attributes::Update(context,
					 name,
					 value) == StatusError)
	escape("unable to update the attribute");

      // answer the caller.
      if (user->application->channel->Reply(Inputs<TagOk>()) == StatusError)
	escape("unable to reply to the application");

      leave();
    }

    ///
    /// this method removes the attribute from the list.
    ///
    Status		Attributes::Remove(const
					     context::Identifier& identifier,
					   const String&	name)
    {
      context::Object*	context;
      user::User*	user;

      enter();

      printf("[XXX] Attributes::Remove()\n");

      // load the current user.
      if (user::User::Instance(user) == StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // retrieve the context.
      if (context::Context::Retrieve(identifier, context) == StatusError)
	escape("unable to retrieve the object context");

      // check if the context is an object.
      if ((context->format & context::FormatObject) !=
	  context::FormatObject)
	escape("unable to test non-objects");

      // request the components.
      if (components::Attributes::Remove(context,
					 name) == StatusError)
	escape("unable to remove the attribute");

      // answer the caller.
      if (user->application->channel->Reply(Inputs<TagOk>()) == StatusError)
	escape("unable to reply to the application");

      leave();
    }

  }
}
