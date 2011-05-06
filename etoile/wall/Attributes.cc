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
// updated       julien quintard   [fri may  6 14:01:28 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/wall/Attributes.hh>

#include <etoile/context/Object.hh>

#include <etoile/components/Attributes.hh>

#include <etoile/user/User.hh>

#include <etoile/Manifest.hh>

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
    elle::Status	Attributes::Set(const
				          context::Identifier&	identifier,
					const elle::String&	name,
					const elle::String&	value)
    {
      context::Object*	context;
      user::User*	user;

      enter();

      printf("[XXX] Attributes::Set()\n");

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
	escape("unable to test non-objects");

      // request the components.
      if (components::Attributes::Set(context,
				      name,
				      value) == elle::StatusError)
	escape("unable to set the attribute");

      // answer the caller.
      if (user->application->channel->Reply(elle::Inputs<TagOk>()) ==
	  elle::StatusError)
	escape("unable to reply to the application");

      leave();
    }

    ///
    /// this method returns the caller the trait associated with
    /// the given name.
    ///
    elle::Status	Attributes::Get(const
					  context::Identifier&	identifier,
					const elle::String&	name)
    {
      context::Object*	context;
      user::User*	user;
      nucleus::Trait*	trait;

      enter();

      printf("[XXX] Attributes::Get()\n");

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
	escape("unable to test non-objects");

      // request the components.
      if (components::Attributes::Get(context, name, trait) ==
	  elle::StatusError)
	escape("unable to retrieve the trait");

      // answer the caller, depending on the result.
      if (trait == NULL)
	{
	  // return the null trait.
	  if (user->application->channel->Reply(
	        elle::Inputs<TagAttributesTrait>(nucleus::Trait::Null)) ==
	      elle::StatusError)
	    escape("unable to reply to the application");
	}
      else
	{
	  // return the trait.
	  if (user->application->channel->Reply(
	        elle::Inputs<TagAttributesTrait>(*trait)) == elle::StatusError)
	    escape("unable to reply to the application");
	}

      leave();
    }

    ///
    /// this method returns the attributes list.
    ///
    elle::Status	Attributes::Fetch(const
					    context::Identifier& identifier)
    {
      context::Object*			context;
      user::User*			user;
      nucleus::Range<nucleus::Trait>	range;

      enter();

      printf("[XXX] Attributes::Fetch()\n");

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
	escape("unable to test non-objects");

      // request the components.
      if (components::Attributes::Fetch(context,
					range) == elle::StatusError)
	escape("unable to fetch the attributes list");

      // answer the caller.
      if (user->application->channel->Reply(
	    elle::Inputs<TagAttributesRange>(range)) == elle::StatusError)
	escape("unable to reply to the application");

      leave();
    }

    ///
    /// this method removes the attribute from the list.
    ///
    elle::Status	Attributes::Omit(const
					   context::Identifier&	identifier,
					 const elle::String&	name)
    {
      context::Object*	context;
      user::User*	user;

      enter();

      printf("[XXX] Attributes::Omit()\n");

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
	escape("unable to test non-objects");

      // request the components.
      if (components::Attributes::Omit(context,
				       name) == elle::StatusError)
	escape("unable to remove the attribute");

      // answer the caller.
      if (user->application->channel->Reply(elle::Inputs<TagOk>()) ==
	  elle::StatusError)
	escape("unable to reply to the application");

      leave();
    }

  }
}
