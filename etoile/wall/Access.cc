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
// updated       julien quintard   [tue apr  6 23:25:42 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/wall/Access.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method tests if the given subject exist in the access list.
    ///
    Status		Access::Exist(const
				        context::Identifier&	identifier,
				      const
				        kernel::Subject&	subject)
    {
      context::Directory*	context;
      user::User*		user;
      Boolean			boolean;

      enter();

      printf("[XXX] Access::Exist()\n");

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
      if (components::Access::Exist(context, subject, boolean) == StatusError)
	escape("unable to know if the given subject exist");

      // answer the caller.
      if (user->application->channel->Reply(
	    Inputs<TagBoolean>(boolean)) == StatusError)
	escape("unable to reply to the application");

      leave();
    }

    ///
    /// this method returns the caller the access record associated with
    /// the given subject.
    ///
    Status		Access::Lookup(const
				         context::Identifier&	identifier,
				       const
				         kernel::Subject&	subject)
    {
      context::Directory*	context;
      user::User*		user;
      kernel::Record		record;

      enter();

      printf("[XXX] Access::Lookup()\n");

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
      if (components::Access::Lookup(context, subject, record) == StatusError)
	escape("unable to retrieve the subject's access record");

      // answer the caller.
      if (user->application->channel->Reply(
	    Inputs<TagAccessRecord>(record)) == StatusError)
	escape("unable to reply to the application");

      leave();
    }

    ///
    /// this method returns a subset of the access list.
    ///
    Status		Access::Consult(const
				          context::Identifier&	identifier,
					const
					  kernel::Index&	index,
					const
					  kernel::Size&		size)
    {
      context::Directory*	context;
      user::User*		user;
      kernel::Range		range;

      enter();

      printf("[XXX] Access::Consult()\n");

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
      if (components::Access::Consult(context,
				      index,
				      size,
				      range) == StatusError)
	escape("unable to consult the access list");

      // answer the caller.
      if (user->application->channel->Reply(
	    Inputs<TagAccessRange>(range)) == StatusError)
	escape("unable to reply to the application");

      leave();
    }

    ///
    /// this method grants access to the given subject, with the given
    /// permissions.
    ///
    Status		Access::Grant(const
				        context::Identifier&	identifier,
				      const
				        kernel::Subject&	subject,
				      const
				        kernel::Permissions&	permissions)
    {
      context::Directory*	context;
      user::User*		user;

      enter();

      printf("[XXX] Access::Grant()\n");

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
      if (components::Access::Grant(context,
				    subject,
				    permissions) == StatusError)
	escape("unable to grant access to the subject");

      // answer the caller.
      if (user->application->channel->Reply(Inputs<TagOk>()) == StatusError)
	escape("unable to reply to the application");

      leave();
    }

    ///
    /// this method updates the permissions of the given subject.
    ///
    Status		Access::Update(const
				         context::Identifier&	identifier,
				       const
				         kernel::Subject&	subject,
				       const
				         kernel::Permissions&	permissions)
    {
      context::Directory*	context;
      user::User*		user;

      enter();

      printf("[XXX] Access::Update()\n");

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
      if (components::Access::Update(context,
				     subject,
				     permissions) == StatusError)
	escape("unable to update the access list");

      // answer the caller.
      if (user->application->channel->Reply(Inputs<TagOk>()) == StatusError)
	escape("unable to reply to the application");

      leave();
    }

    ///
    /// this method blocks the given subject from accessing the object.
    ///
    Status		Access::Block(const
				        context::Identifier&	identifier,
				      const
				        kernel::Subject&	subject)
    {
      context::Directory*	context;
      user::User*		user;

      enter();

      printf("[XXX] Access::Block()\n");

      leave();
    }

    ///
    /// this method removes the permissions from the given subject.
    ///
    Status		Access::Revoke(const
				         context::Identifier&	identifier,
				       const
				         kernel::Subject&	subject)
    {
      context::Directory*	context;
      user::User*		user;

      enter();

      printf("[XXX] Access::Revoke()\n");

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
      if (components::Access::Revoke(context,
				     subject) == StatusError)
	escape("unable to revoke the subject's permissions");

      // answer the caller.
      if (user->application->channel->Reply(Inputs<TagOk>()) == StatusError)
	escape("unable to reply to the application");

      leave();
    }

  }
}
