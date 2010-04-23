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
// updated       julien quintard   [fri apr 23 11:13:09 2010]
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
    /// this method sets a trait to the object's attributes.
    ///
    Status		Attributes::Set(const
				          context::Identifier&	identifier,
					const String&		name,
					const String&		value)
    {
      context::Object*	context;
      user::User*	user;

      enter();

      printf("[XXX] Attributes::Set()\n");

      // load the current user.
      if (user::User::Instance(user) == StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // retrieve the context.
      if (user->application->Retrieve(identifier, context) == StatusError)
	escape("unable to retrieve the object context");

      // check if the context is an object.
      if ((context->format & context::FormatObject) !=
	  context::FormatObject)
	escape("unable to test non-objects");

      // request the components.
      if (components::Attributes::Set(context,
				      name,
				      value) == StatusError)
	escape("unable to set the attribute");

      // answer the caller.
      if (user->application->channel->Reply(Inputs<TagOk>()) == StatusError)
	escape("unable to reply to the application");

      leave();
    }

    ///
    /// this method returns the caller the trait associated with
    /// the given name.
    ///
    Status		Attributes::Get(const
					  context::Identifier&	identifier,
					const String&		name)
    {
      context::Object*	context;
      user::User*	user;
      kernel::Trait*	trait;

      enter();

      printf("[XXX] Attributes::Get()\n");

      // load the current user.
      if (user::User::Instance(user) == StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // retrieve the context.
      if (user->application->Retrieve(identifier, context) == StatusError)
	escape("unable to retrieve the object context");

      // check if the context is an object.
      if ((context->format & context::FormatObject) !=
	  context::FormatObject)
	escape("unable to test non-objects");

      // request the components.
      if (components::Attributes::Get(context, name, trait) == StatusError)
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
    /// this method returns the attributes list.
    ///
    Status		Attributes::Fetch(const
					    context::Identifier& identifier)
    {
      context::Object*			context;
      user::User*			user;
      kernel::Range<kernel::Trait>	range;

      enter();

      printf("[XXX] Attributes::Fetch()\n");

      // load the current user.
      if (user::User::Instance(user) == StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // retrieve the context.
      if (user->application->Retrieve(identifier, context) == StatusError)
	escape("unable to retrieve the object context");

      // check if the context is an object.
      if ((context->format & context::FormatObject) !=
	  context::FormatObject)
	escape("unable to test non-objects");

      // request the components.
      if (components::Attributes::Fetch(context,
					range) == StatusError)
	escape("unable to fetch the attributes list");

      // answer the caller.
      if (user->application->channel->Reply(
	    Inputs<TagAttributesRange>(range)) == StatusError)
	escape("unable to reply to the application");

      leave();
    }

    ///
    /// this method removes the attribute from the list.
    ///
    Status		Attributes::Omit(const
					   context::Identifier&	identifier,
					 const String&		name)
    {
      context::Object*	context;
      user::User*	user;

      enter();

      printf("[XXX] Attributes::Omit()\n");

      // load the current user.
      if (user::User::Instance(user) == StatusError)
	escape("unable to load the user");

      // check if the user is an application..
      if (user->type != user::User::TypeApplication)
	escape("non-applications cannot authenticate");

      // retrieve the context.
      if (user->application->Retrieve(identifier, context) == StatusError)
	escape("unable to retrieve the object context");

      // check if the context is an object.
      if ((context->format & context::FormatObject) !=
	  context::FormatObject)
	escape("unable to test non-objects");

      // request the components.
      if (components::Attributes::Omit(context,
				       name) == StatusError)
	escape("unable to remove the attribute");

      // answer the caller.
      if (user->application->channel->Reply(Inputs<TagOk>()) == StatusError)
	escape("unable to reply to the application");

      leave();
    }

  }
}
