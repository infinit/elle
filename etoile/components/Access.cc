//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/components/Access.cc
//
// created       julien quintard   [mon feb  1 19:24:19 2010]
// updated       julien quintard   [tue feb  2 22:36:00 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/components/Access.hh>

namespace etoile
{
  namespace components
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    Status		Access::Open(context::Object*		context)
    {
      // if the access is already opened, return.
      if (context->access != NULL)
	leave();

      // check if there exists an access. if so, load the block.
      if (context->object->meta.access != hole::Address::Null)
	{
	  // load the block.
	  if (depot::Depot::Get(context->object->meta.access,
				context->access) == StatusError)
	    escape("unable to load the access");
	}
      else
	{
	  // otherwise create a new access.
	  context->access = new core::Access;
	}

      leave();
    }

    ///
    /// XXX
    ///
    Status		Access::Close(context::Object*		context)
    {
      core::Offset	size;

      // if there is no loaded access, then there is nothing to do.
      if (context->access == NULL)
	leave();

      // retrieve the access's size.
      if (context->access->Size(size) == StatusError)
	escape("unable to retrieve the access's size");

      // if the access has not changed, or if the access is empty,
      // delete it.
      if ((size == 0) || (context->access->state == StateClean))
	{
	  // release the access's memory.
	  delete context->access;

	  // reset the pointer.
	  context->access = NULL;

	  leave();
	}

      // bind the access as, since the block has changed, its address
      // is going to be different.
      if (context->access->Bind() == StatusError)
	escape("unable to bind the access");

      // finally, update the object's meta section with the new address.
      if (context->object->Administrate(context->access->address,
					context->object->meta.owner.permissions,
					context->object->meta.owner.token) ==
	  StatusError)
	escape("unable to update the object's meta section");
					
      leave();
    }

    ///
    /// XXX
    ///
    Status		Access::Retrieve(context::Object*	context,
					 const core::Subject&	subject,
					 core::Permissions&	permissions,
					 Code&			token)
    {
      // test if the subject is the object's owner.
      if ((subject.type == core::Subject::TypeUser) &&
	  (*subject.identifier.user == context->object->owner.K))
	{
	  // return the permissions.
	  permissions = context->object->meta.owner.permissions;

	  // along with the token.
	  token = context->object->meta.owner.token;
	}
      else
	{
	  // XXX delegate or vassal
	  printf("XXX[NOT IMPLEMENTED YET]\n");
	}

      leave();
    }

    ///
    /// XXX
    ///
    Status		Access::Upgrade(context::Object*	context,
					const SecretKey&	key)
    {
      //
      // first, go through all the access entries and upgrade the key.
      //
      {
	core::Access::Iterator	iterator;

	// for each subject in the access block.
	for (iterator = context->access->entries.begin();
	     iterator != context->access->entries.end();
	     iterator++)
	  {
	    core::Access::Entry*	entry = *iterator;

	    // check if the subject has the proper permissions.
	    if (!(entry->permissions & core::PermissionRead))
	      continue;

	    // depending on the subject's type.
	    switch (entry->subject.type)
	      {
	      case core::Subject::TypeUser:
		{
		  //
		  // if the subject is a user, encrypt the key with the
		  // user's public key so that she will be the only one
		  // capable of decrypting it.
		  //

		  PublicKey*	K;
		  Code		token;

		  // retrieve the public key.
		  K = entry->subject.identifier.user;

		  // encrypt the secret key.
		  if (K->Encrypt(key, token) == StatusError)
		    escape("unable to encrypt the key with the "
			   "user's public key");

		  // assign the token to the access' entry.
		  entry->token = token;

		  break;
		}
	      case core::Subject::TypeGroup:
		{
		  //
		  // if the subject is a group, the key is made available
		  // to the group's owner. this is especially useful in
		  // order to increase the number of delegates available to
		  // respond to vassal's requests.
		  //

		  PublicKey*		K;
		  hole::Address*	address;
		  Code			token;

		  // retrieve the group's block address.
		  // XXX

		  // retrieve the owner's public key.
		  // XXX

		  // encrypt the secret key.
		  // XXX

		  // assign the token to the entry.
		  // XXX

		  break;
		}
	      case core::Subject::TypeUnknown:
		{
		  escape("the access block contains unknown entries");
		}
	      }

	    // set the access block as being dirty.
	    context->access->state = core::StateDirty;
	  }
      }

      //
      // then, update the object's owner, assuming she has the proper
      // permissions and modify the object.
      //
      // at this point, do not worry for the access block's address as
      // the new address will be computed once the access is closed and
      // the object will be updated accordingly. the code below just updates
      // the token.
      //
      {
	Code		token;

	// check if the owner has the proper permissions.
	if (context->object->meta.owner.permissions & core::PermissionRead)
	  {
	    // encrypt the secret key with the owner's public key.
	    if (context->object->owner.K.Encrypt(key, token) == StatusError)
	      escape("unable to encrypt the key with the owner's public key");

	    // finally, update the object's meta section though the
	    // access address and owner permissions remain unchanged.
	    if (context->object->Administrate(context->object->meta.access,
					      context->object->meta.owner.permissions,
					      token) == StatusError)
	      escape("unable to update the object's meta section");
	  }
      }

      leave();
    }

  }
}
