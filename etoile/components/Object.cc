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
// updated       julien quintard   [tue apr 27 20:41:27 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/components/Object.hh>

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
    Status		Object::Load(context::Object*		context,
				     const hole::Address&	address)
    {
      enter();

      // set the object address.
      context->address = address;

      // get the block.
      if (depot::Depot::Get(address, context->object) == StatusError)
	escape("unable to retrieve the block");

      leave();
    }

    ///
    /// this method fills the state object with general-purpose information
    /// on the object.
    ///
    Status		Object::Information(context::Object*	context,
					    wall::State&	state)
    {
      enter();

      // create the state based on the object.
      if (state.Create(*context->object) == StatusError)
	escape("unable to create the state");

      leave();
    }

    ///
    /// this method stores the object.
    ///
    Status		Object::Store(context::Object*		context)
    {
      user::User*	user;

      enter();

      // load the current user.
      if (user::User::Instance(user) == StatusError)
	escape("unable to load the current user");

      // close the access.
      if (Access::Close(context) == StatusError)
	escape("unable to close the access");

      // if the object has been modified, seal it and record it.
      if ((context->object->data.state != kernel::StateClean) ||
	  (context->object->meta.state != kernel::StateClean))
	{
	  // seal the object.
	  if (context->object->Seal(*user->client->agent,
				    context->access) == StatusError)
	    escape("unable to seal the object");

	  // record the object in the bucket.
	  if (context->bucket.Push(
		context->object->address,
		context->object) == StatusError)
	    escape("unable to record the object block in the bucket");
	}

      // record the context in the journal.
      if (journal::Journal::Record(context) == StatusError)
	escape("unable to the context");

      leave();
    }

    ///
    /// this method discards the modifications.
    ///
    Status		Object::Discard(context::Object*	context)
    {
      enter();

      // import the context so that it no longer is usable by the
      // application.
      if (context::Context::Import(context) == StatusError)
	escape("unable to import the context");

      // delete the context.
      if (context::Context::Delete(context) == StatusError)
	escape("unable to delete the context");

      leave();
    }

    ///
    /// this method destroys the object.
    ///
    Status		Object::Destroy(context::Object*	context)
    {
      user::User*	user;

      enter();

      // destroy the access.
      if (Access::Destroy(context) == StatusError)
	escape("unable to destroy the access");

      // record the object in the bucket.
      if (context->bucket.Destroy(
	    context->object->address) == StatusError)
	escape("unable to record the object block in the bucket");

      // record the context in the journal.
      if (journal::Journal::Record(context) == StatusError)
	escape("unable to the context");

      leave();
    }

  }
}
