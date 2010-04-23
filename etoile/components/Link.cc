//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/components/Link.cc
//
// created       julien quintard   [fri aug 14 19:00:57 2009]
// updated       julien quintard   [thu apr 22 11:10:36 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/components/Link.hh>

namespace etoile
{
  namespace components
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a link object.
    ///
    Status		Link::Create(context::Link*		context)
    {
      user::User*	user;

      enter();

      // load the current user.
      if (user::User::Instance(user) == StatusError)
	escape("unable to load the user");

      // allocate a new link object.
      context->object = new kernel::Object;

      // create the irectory.
      if (context->object->Create(kernel::GenreLink,
				  user->client->agent->K) == StatusError)
	escape("unable to create the link object");

      // bind the object.
      if (context->object->Bind() == StatusError)
	escape("unable to bind the object");

      // set the address.
      context->address = context->object->address;

      leave();
    }

    ///
    /// this method loads the link object identified by the given
    /// address in the context.
    ///
    Status		Link::Load(context::Link*		context,
				   const hole::Address&		address)
					
    {
      enter();

      // load the object.
      if (Object::Load(context, address) == StatusError)
	escape("unable to load the object");

      // check that the object is a link.
      if (context->object->meta.genre != kernel::GenreLink)
	escape("this object is not a link");

      leave();
    }

    ///
    /// this method binds a new target to the link.
    ///
    Status		Link::Bind(context::Link*		context,
				   const path::Way&		way)
    {
      enter();

      // determine the rights.
      if (Rights::Determine(context) == StatusError)
	escape("unable to determine the rights");

      // check if the current user has the right the write the reference.
      if (!(context->rights->record.permissions & kernel::PermissionWrite))
	escape("unable to perform the operation without the permission");

      // open the contents.
      if (Contents::Open(context) == StatusError)
	escape("unable to open the contents");

      // bind the link.
      if (context->contents->content->Bind(way) == StatusError)
	escape("unable to bind the link");

      leave();
    }

    ///
    /// this method returns the way associated with this link.
    ///
    Status		Link::Resolve(context::Link*		context,
				      path::Way&		way)
    {
      enter();

      // determine the rights.
      if (Rights::Determine(context) == StatusError)
	escape("unable to determine the rights");

      // check if the current user has the right the read the reference.
      if (!(context->rights->record.permissions & kernel::PermissionRead))
	escape("unable to perform the operation without the permission");

      // open the contents.
      if (Contents::Open(context) == StatusError)
	escape("unable to open the contents");

      // resolve the link.
      if (context->contents->content->Resolve(way) == StatusError)
	escape("unable to resolve the link");

      leave();
    }

    ///
    /// this method discards the modifications applied onto the context.
    ///
    Status		Link::Discard(context::Link*		context)
    {
      enter();

      // discard the object's modifications.
      if (Object::Discard(context) == StatusError)
	escape("unable to discard the object modifications");

      leave();
    }

    ///
    /// this store the modifications applied onto the link context.
    ///
    Status		Link::Store(context::Link*		context)
    {
      user::User*	user;

      enter();

      // close the catalog.
      if (Contents::Close(context) == StatusError)
	escape("unable to close the contents");

      // store the object.
      if (Object::Store(context) == StatusError)
	escape("unable to store the object");

      leave();
    }

    ///
    /// this method removes the object along with the blocks attached to it.
    ///
    Status		Link::Destroy(context::Link*		context)
    {
      user::User*	user;
      kernel::Size	size;

      enter();

      // determine the rights.
      if (Rights::Determine(context) == StatusError)
	escape("unable to determine the rights");

      // check if the current user is the object owner.
      if (context->rights->role != kernel::RoleOwner)
	escape("unable to destroy a not-owned object");

      // destroy the contents.
      if (Contents::Destroy(context) == StatusError)
	escape("unable to destroy the contents");

      // destroy the object.
      if (Object::Destroy(context) == StatusError)
	escape("unable to destroy the object");

      leave();
    }

  }
}
