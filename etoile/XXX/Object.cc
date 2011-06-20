//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/XXX/Object.cc
//
// created       julien quintard   [fri aug 14 19:16:10 2009]
// updated       julien quintard   [thu jun 16 15:07:01 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/components/Object.hh>
#include <etoile/components/Access.hh>

#include <etoile/user/User.hh>

#include <etoile/depot/Depot.hh>

#include <etoile/journal/Journal.hh>

#include <etoile/context/Context.hh>

namespace etoile
{
  namespace components
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method loads an object in the given context.
    ///
    elle::Status	Object::Load(context::Object*		context,
				     const nucleus::Address&	address)
    {
      user::User*	user;

      enter();

      // load the current user.
      if (user::User::Instance(user) == elle::StatusError)
	escape("unable to load the user");

      // set the object address.
      context->address = address;

      // allocate a new Object block.
      context->object = new nucleus::Object;

      // get the block.
      if (depot::Depot::Pull(user->application->network,
			     address,
			     nucleus::Version::Last,
			     *context->object) == elle::StatusError)
	escape("unable to retrieve the block");

      leave();
    }

    ///
    /// this method fills the status object with general-purpose information
    /// on the object.
    ///
    elle::Status	Object::Information(context::Object*	context,
					    wall::Status&	status)
    {
      enter();

      // create the status based on the object.
      if (status.Create(*context->object) == elle::StatusError)
	escape("unable to create the status");

      leave();
    }

    ///
    /// this method stores the object.
    ///
    elle::Status	Object::Store(context::Object*		context)
    {
      user::User*	user;

      enter();

      // load the current user.
      if (user::User::Instance(user) == elle::StatusError)
	escape("unable to load the current user");

      // close the access.
      if (Access::Close(context) == elle::StatusError)
	escape("unable to close the access");

      // if the object has been modified, seal it and record it.
      if ((context->object->data.state != nucleus::StateClean) ||
	  (context->object->meta.state != nucleus::StateClean))
	{
	  // seal the object.
	  if (context->object->Seal(*user->client->agent,
				    context->access) == elle::StatusError)
	    escape("unable to seal the object");

	  // record the object in the bucket.
	  if (context->bucket.Push(
		context->address,
		context->object) == elle::StatusError)
	    escape("unable to record the object block in the bucket");
	}

      // record the context in the journal.
      if (journal::Journal::Record(context) == elle::StatusError)
	escape("unable to the context");

      leave();
    }

    ///
    /// this method discards the modifications.
    ///
    elle::Status	Object::Discard(context::Object*	context)
    {
      enter();

      // import the context so that it no longer is usable by the
      // application.
      if (context::Context::Import(context) == elle::StatusError)
	escape("unable to import the context");

      // delete the context.
      if (context::Context::Delete(context) == elle::StatusError)
	escape("unable to delete the context");

      leave();
    }

    ///
    /// this method destroys the object.
    ///
    elle::Status	Object::Destroy(context::Object*	context)
    {
      user::User*	user;

      enter();

      // destroy the access.
      if (Access::Destroy(context) == elle::StatusError)
	escape("unable to destroy the access");

      // record the object in the bucket.
      if (context->bucket.Wipe(
	    context->address) == elle::StatusError)
	escape("unable to record the object block in the bucket");

      // record the context in the journal.
      if (journal::Journal::Record(context) == elle::StatusError)
	escape("unable to the context");

      leave();
    }

  }
}
