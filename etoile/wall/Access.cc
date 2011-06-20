//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/wall/Access.cc
//
// created       julien quintard   [wed mar 31 19:26:06 2010]
// updated       julien quintard   [tue jun 14 14:53:09 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/wall/Access.hh>

#include <etoile/gear/Identifier.hh>

// XXX #include <etoile/context/Object.hh>
// #include <etoile/context/Format.hh>

// XXX #include <etoile/components/Access.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method returns the caller the access record associated with
    /// the given subject.
    ///
    elle::Status	Access::Lookup(
			  const gear::Identifier&		identifier,
			  const nucleus::Subject&		subject,
			  nucleus::Record&			record)
    {
      /*
      context::Object*	context;
      user::User*	user;
      nucleus::Record*	record;

      enter();

      printf("[XXX] Access::Lookup()\n");

      // XXX retrieve context

      // retrieve the context.
      if (user->application->Retrieve(identifier, context) ==
	  elle::StatusError)
	escape("unable to retrieve the object context");

      // check if the context is an object.
      if ((context->format & context::FormatObject) !=
	  context::FormatObject)
	escape("unable to test non-objects");

      // request the components.
      if (components::Access::Lookup(context, subject, record) ==
	  elle::StatusError)
	escape("unable to retrieve the subject's access record");

      // answer the caller, depending on the result.
      if (record == NULL)
	{
	  // return the null record.
	  if (user->application->channel->Reply(
	        elle::Inputs<TagAccessRecord>(nucleus::Record::Null)) ==
	      elle::StatusError)
	    escape("unable to reply to the application");
	}
      else
	{
	  // return the record.
	  if (user->application->channel->Reply(
	        elle::Inputs<TagAccessRecord>(*record)) == elle::StatusError)
	    escape("unable to reply to the application");
	}

      leave();
      */
    }

    ///
    /// this method returns a subset of the access list.
    ///
    elle::Status	Access::Consult(
			  const gear::Identifier&		identifier,
			  const nucleus::Index&			index,
			  const nucleus::Size&			size,
			  nucleus::Range<nucleus::Record>&	range)
    {
      /*
      context::Object*			context;
      user::User*			user;
      nucleus::Range<nucleus::Record>	range;

      enter();

      printf("[XXX] Access::Consult()\n");

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
      if (components::Access::Consult(context,
				      index,
				      size,
				      range) == elle::StatusError)
	escape("unable to consult the access list");

      // answer the caller.
      if (user->application->channel->Reply(
	    elle::Inputs<TagAccessRange>(range)) == elle::StatusError)
	escape("unable to reply to the application");

      leave();
      */
    }

    ///
    /// this method grants access to the given subject, with the given
    /// permissions.
    ///
    elle::Status	Access::Grant(
			  const gear::Identifier&		identifier,
			  const nucleus::Subject&		subject,
			  const nucleus::Permissions&		permissions)
    {
      /*
      context::Object*	context;
      user::User*	user;

      enter();

      printf("[XXX] Access::Grant()\n");

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
      if (components::Access::Grant(context,
				    subject,
				    permissions) == elle::StatusError)
	escape("unable to grant access to the subject");

      // answer the caller.
      if (user->application->channel->Reply(elle::Inputs<TagOk>()) ==
	  elle::StatusError)
	escape("unable to reply to the application");

      leave();
      */
    }

    ///
    /// this method updates the permissions of the given subject.
    ///
    elle::Status	Access::Update(
			  const gear::Identifier&		identifier,
			  const nucleus::Subject&		subject,
			  const nucleus::Permissions&		permissions)
    {
      /*
      context::Object*	context;
      user::User*	user;

      enter();

      printf("[XXX] Access::Update()\n");

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
      if (components::Access::Update(context,
				     subject,
				     permissions) == elle::StatusError)
	escape("unable to update the access list");

      // answer the caller.
      if (user->application->channel->Reply(elle::Inputs<TagOk>()) ==
	  elle::StatusError)
	escape("unable to reply to the application");

      leave();
      */
    }

    ///
    /// this method blocks the given subject from accessing the object.
    ///
    elle::Status	Access::Block(
			  const gear::Identifier&		identifier,
			  const nucleus::Subject&		subject)
    {
      enter();

      printf("[XXX] Access::Block()\n");

      leave();
    }

    ///
    /// this method removes the permissions from the given subject.
    ///
    elle::Status	Access::Revoke(
			  const gear::Identifier&		identifier,
			  const nucleus::Subject&		subject)
    {
      /*
      context::Object*	context;
      user::User*	user;

      enter();

      printf("[XXX] Access::Revoke()\n");

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
      if (components::Access::Revoke(context,
				     subject) == elle::StatusError)
	escape("unable to revoke the subject's permissions");

      // answer the caller.
      if (user->application->channel->Reply(elle::Inputs<TagOk>()) ==
	  elle::StatusError)
	escape("unable to reply to the application");

      leave();
      */
    }

  }
}
