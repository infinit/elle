//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/components/Object.cc
//
// created       julien quintard   [fri aug 14 19:16:10 2009]
// updated       julien quintard   [tue may  4 11:01:03 2010]
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
				     const hole::Address&	address)
    {
      enter();

      // set the object address.
      context->address = address;

      // get the block.
      if (depot::Depot::Get(address, context->object) == elle::StatusError)
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
      if ((context->object->data.state != kernel::StateClean) ||
	  (context->object->meta.state != kernel::StateClean))
	{
	  // seal the object.
	  if (context->object->Seal(*user->client->agent,
				    context->access) == elle::StatusError)
	    escape("unable to seal the object");

	  // record the object in the bucket.
	  if (context->bucket.Push(
		context->object->address,
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
      if (context->bucket.Destroy(
	    context->object->address) == elle::StatusError)
	escape("unable to record the object block in the bucket");

      // record the context in the journal.
      if (journal::Journal::Record(context) == elle::StatusError)
	escape("unable to the context");

      leave();
    }

  }
}
