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
// updated       julien quintard   [fri apr 16 15:08:10 2010]
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
    /// this method opens i.e load, the access block if present or
    /// creates an empty new one otherwise.
    ///
    Status		Access::Open(context::Object*		context)
    {
      enter();

      // if the access is already opened, return.
      if (context->access != NULL)
	leave();

      // check if there exists an access block. if so, load the block.
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
	  context->access = new kernel::Access;
	}

      leave();
    }

    ///
    /// this method adds a record to the access block, granting access
    /// to the given subject.
    ///
    Status		Access::Grant(context::Object*		context,
				      const kernel::Subject&	subject,
				      const kernel::Permissions& permissions)
    {
      user::User*	user;
      kernel::Record*	record;

      enter(instance(record));

      // determine the rights over the object.
      if (Rights::Determine(context) == StatusError)
	escape("unable to determine the rights");

      // verify that the user can modify the accesses.
      if (context->rights->role != kernel::RoleOwner)
	escape("the object's owner only can modify the accesses");

      // look in the access object.
      if (context->access->Exist(subject) == StatusTrue)
	escape("this subject already exist in the access block");

      // update the access block or object according to the subject.
      if (subject == context->object->owner.subject)
	{
	  //
	  // in this case, the subject represents the object's owner.
	  //

	  // update the permissions.
	  if (context->object->Administrate(
                context->object->meta.attributes,
		context->object->meta.access,
		permissions,
		context->object->meta.owner.token) == StatusError)
	    escape("unable to update the object's meta section");
	}
      else
	{
	  //
	  // otherwise, the subject is a delegate being a user or a group.
	  //

	  // open the access.
	  if (Access::Open(context) == StatusError)
	    escape("unable to open the access");

	  // allocate a new record.
	  record = new kernel::Record;

	  // create the new record.
	  if (record->Update(subject,
			     permissions,
			     context->rights->key) == StatusError)
	    escape("unable to create the new record");

	  // add the record to the access object.
	  if (context->access->Add(record) == StatusError)
	    escape("unable to add the new record");

	  // stop tracking record.
	  waive(record);
	}

      // load the current user.
      if (user::User::Instance(user) == StatusError)
	escape("unable to load the user");

      // check if the subject is the user.
      if (user->client->subject == subject)
	{
	  // update the context rights.
	  if (Rights::Update(context, permissions) == StatusError)
	    escape("unable to update the rights' permissions");
	}

      leave();
    }

    ///
    /// this method looks for the given subject in the access block and
    /// return the associated permissions.
    ///
    Status		Access::Lookup(context::Object*		context,
				       const kernel::Subject&	subject,
				       kernel::Record*&		record)
    {
      user::User*	user;

      enter();

      // initialize the record to NULL.
      record = NULL;

      // load the current user.
      if (user::User::Instance(user) == StatusError)
	escape("unable to load the user");

      // try to make the best of this call.
      if (user->client->subject == subject)
	{
	  //
	  // indeed, if the target subject is the current user, determine
	  // the user's right so that this is not to be done later.
	  //

	  // determine the user's rights on the object.
	  if (Rights::Determine(context) == StatusError)
	    escape("unable to determine the user's rights");

	  // set the record attributes from the user's rights.
	  record = &context->rights->record;
	}
      else
	{
	  //
	  // otherwise, proceed normally.
	  //

	  // perform the lookup according to the subject.
	  if (subject == context->object->owner.subject)
	    {
	      //
	      // if the target subject is the object owner, the access
	      // permissions etc. must be retrieved from the object
	      // meta section.
	      //

	      // return the owner access record.
	      record = &context->object->meta.owner.record;
	    }
	  else
	    {
	      //
	      // if we are dealing with a delegate, open the access block
	      // in look in it.
	      //

	      // open the access.
	      if (Access::Open(context) == StatusError)
		escape("unable to open the access");

	      // check if the record exists.
	      if (context->access->Exist(subject) == StatusFalse)
		leave();

	      // lookup in the access object.
	      if (context->access->Lookup(subject, record) == StatusError)
		escape("unable to lookup in the access object");
	    }
	}

      leave();
    }

    ///
    /// this method returns a subset---i.e a range---of the access block.
    ///
    Status		Access::Consult(context::Object*	context,
					const kernel::Index&	index,
					const kernel::Size&	size,
					kernel::Range&		range)
    {
      enter();

      // open the access.
      if (Access::Open(context) == StatusError)
	escape("unable to open the access");

      // if the index starts with 0, including the owner by creating
      // a record for her.
      if (index == 0)
	{
	  kernel::Record*	record;
	  kernel::Subject	subject;

	  enter(instance(record));

	  // create a subject for the owner.
	  if (subject.Create(context->object->owner.K) == StatusError)
	    escape("unable to create the owner subject");

	  // create the record.
	  record = new kernel::Record(subject,
				      context->object->meta.owner.permissions,
				      context->object->meta.owner.token);

	  // add the record to the range.
	  if (range.Add(record) == StatusError)
	    escape("unable to add the owner record");

	  // waive.
	  waive(record);

	  // consult the access object by taking care of consulting one
	  // record less.
	  if (context->access->Consult(index, size - 1, range) == StatusError)
	    escape("unable to consult the access object");
	}
      else
	{
	  // consult the access object by taking care of starting the
	  // consultation one index before.
	  if (context->access->Consult(index -1 , size, range) == StatusError)
	    escape("unable to consult the access object");
	}

      leave();
    }

    ///
    /// this method updates an existing record with the given permissions.
    ///
    Status		Access::Update(context::Object*		context,
				       const kernel::Subject&	subject,
				       const kernel::Permissions& permissions)
    {
      user::User*	user;

      enter();

      // determine the rights over the object.
      if (Rights::Determine(context) == StatusError)
	escape("unable to determine the rights");

      // verify that the user can modify the accesses.
      if (context->rights->role != kernel::RoleOwner)
	escape("the object's owner only can modify the accesses");

      // update the access block or object according to the subject.
      if (subject == context->object->owner.subject)
	{
	  //
	  // in this case, the subject represents the object's owner.
	  //

	  // update the permissions.
	  if (context->object->Administrate(
                context->object->meta.attributes,
		context->object->meta.access,
		permissions,
		context->object->meta.owner.token) == StatusError)
	    escape("unable to update the object's meta section");
	}
      else
	{
	  //
	  // otherwise, the subject is a delegate being a user or a group.
	  //
	  kernel::Record*	record;

	  // open the access.
	  if (Access::Open(context) == StatusError)
	    escape("unable to open the access");

	  // retrieve the record associated with the given subject.
	  if (context->access->Lookup(subject, record) == StatusTrue)
	    escape("unable to retrive the subject's access record");

	  // update the record.
	  if (record->Update(subject,
			     permissions,
			     context->rights->key) == StatusError)
	    escape("unable to update the subject's record");
	}

      // load the current user.
      if (user::User::Instance(user) == StatusError)
	escape("unable to load the user");

      // check if the subject is the user.
      if (user->client->subject == subject)
	{
	  // update the context rights.
	  if (Rights::Update(context, permissions) == StatusError)
	    escape("unable to update the rigths' permissions");
	}

      leave();
    }

    ///
    /// this method blocks a subject by recording it in a black list.
    ///
    Status		Access::Block(context::Object*		context,
				      const kernel::Subject&	subject)
    {
      user::User*	user;

      enter();

      // determine the rights over the object.
      if (Rights::Determine(context) == StatusError)
	escape("unable to determine the rights");

      // verify that the user can modify the accesses.
      if (context->rights->role != kernel::RoleOwner)
	escape("the object's owner only can modify the accesses");

      // XXX record in black list.

      // load the current user.
      if (user::User::Instance(user) == StatusError)
	escape("unable to load the user");

      // check if the subject is the user.
      if (user->client->subject == subject)
	{
	  // update the context rights.
	  if (Rights::Update(context, kernel::PermissionNone) == StatusError)
	    escape("unable to update the rights' permissions");
	}

      leave();
    }

    ///
    /// this method revokes a subject's access by updating the access
    /// block.
    ///
    Status		Access::Revoke(context::Object*		context,
				       const kernel::Subject&	subject)
    {
      user::User*	user;

      enter();

      // determine the rights over the object.
      if (Rights::Determine(context) == StatusError)
	escape("unable to determine the rights");

      // verify that the user can modify the accesses.
      if (context->rights->role != kernel::RoleOwner)
	escape("the object's owner only can modify the accesses");

      // update the access block or object according to the subject.
      if (subject == context->object->owner.subject)
	{
	  //
	  // in this case, the subject represents the object's owner.
	  //

	  // update the permissions.
	  if (context->object->Administrate(
                context->object->meta.attributes,
		context->object->meta.access,
		kernel::PermissionNone,
		context->object->meta.owner.token) == StatusError)
	    escape("unable to update the object's meta section");
	}
      else
	{
	  //
	  // otherwise, the subject is a delegate being a user or a group.
	  //
	  kernel::Record*	record;

	  // open the access.
	  if (Access::Open(context) == StatusError)
	    escape("unable to open the access");

	  // remove the record associated with the given subject.
	  if (context->access->Remove(subject) == StatusTrue)
	    escape("unable to remove the subject's access record");
	}

      // load the current user.
      if (user::User::Instance(user) == StatusError)
	escape("unable to load the user");

      // check if the subject is the user.
      if (user->client->subject == subject)
	{
	  // update the context rights.
	  if (Rights::Update(context, kernel::PermissionNone) == StatusError)
	    escape("unable to update the rights' permissions");
	}

      leave();
    }

    ///
    /// this method is called whenever an object's content has been modified,
    /// leading to the generation of a new key.
    ///
    /// this key must then be distributed to every subject authorised to
    /// access the object in reading, under the form of a token.
    ///
    Status		Access::Upgrade(context::Object*	context,
					const SecretKey&	key)
    {
      enter();

      // upgrate the access block records.
      if (context->access->Upgrade(key) == StatusError)
	escape("unable to upgrade the access records");

      // then, update the object's owner token.
      //
      // noteworthy is that the owner's token is always computed even though
      // she may not have the permission to read. this is required if the
      // owner wants to grant herself back or anyone else the permission
      // to read.
      if (context->object->meta.owner.token.Update(
            key,
	    context->object->owner.K) == StatusError)
	escape("unable to update the owner's token");

      // finally, update the object's meta section though the
      // access address and owner permissions remain unchanged.
      if (context->object->Administrate(
	    context->object->meta.attributes,
            context->object->meta.access,
	    context->object->meta.owner.permissions,
	    context->object->meta.owner.token) == StatusError)
	escape("unable to update the object's meta section");

      // update the rights' token/key if the user has the permission to
      // read the contents.
      if (context->rights->record.permissions & kernel::PermissionRead)
	{
	  // XXX for now, just disable the token so that an error occurs.
	  SecretKey k;

	  context->rights->record.token = kernel::Token::Null;
	  context->rights->key = k;
	}

      leave();
    }

    ///
    /// this method destroys the access block.
    ///
    Status		Access::Destroy(context::Object*	context)
    {
      enter();

      // if the block is present.
      if (context->object->meta.access != hole::Address::Null)
	{
	  // record the block as needed to be removed.
	  if (context->bucket.Destroy(
	        context->object->meta.access) == StatusError)
	    escape("unable to record the block in the bucket");

	  // update the object's meta section with the null address.
	  if (context->object->Administrate(
		context->object->meta.attributes,
		hole::Address::Null,
		context->object->meta.owner.permissions,
		context->object->meta.owner.token) == StatusError)
	    escape("unable to update the object's meta section");

	  // release the context access object.
	  delete context->access;

	  // reset the pointer.
	  context->access = NULL;
	}

      leave();
    }

    ///
    /// this method closes the access. either the access has been modified
    /// and therefore modifications eed to be published or not, in which
    /// case nothing is done.
    ///
    Status		Access::Close(context::Object*		context)
    {
      kernel::Size	size;

      enter();

      // if there is no loaded access, then there is nothing to do.
      if (context->access == NULL)
	leave();

      // if the access has not changed.
      if (context->access->state == kernel::StateClean)
	{
	  // release the access's memory.
	  delete context->access;

	  // reset the pointer.
	  context->access = NULL;

	  leave();
	}

      // retrieve the access's size.
      if (context->access->Capacity(size) == StatusError)
	escape("unable to retrieve the access's size");

      //
      // at this point, this access block is known to have been modified.
      //
      // modify the object according to the content of the access block.
      //
      if (size == 0)
	{
	  //
	  // if access became empty after record removals, the
	  // object should no longer point to any access block while
	  // the old block should be deleted.
	  //

	  // destroy the access block and update the object accordingly.
	  if (Access::Destroy(context) == StatusError)
	    escape("unable to destroy the access block");
	}
      else
	{
	  //
	  // otherwise, compute the address of the new access block and
	  // update the object accordingly.
	  //

	  // delete the previous access block, should one exist.
	  if (context->object->meta.access != hole::Address::Null)
	    {
	      // record the access so that it is destroyed.
	      if (context->bucket.Destroy(
		    context->object->meta.access) == StatusError)
		  escape("unable to record the access block in the bucket");
	      }

	  // bind the access as, since the block has changed, its address
	  // is going to be different.
	  if (context->access->Bind() == StatusError)
	    escape("unable to bind the access");

	  // register the block as needed to be published.
	  if (context->bucket.Push(
		context->access->address,
		context->access) == StatusError)
	    escape("unable to record the access block in the bucket");

	  // set the state as clean.
	  context->access->state = kernel::StateClean;

	  // finally, update the object's meta section with the new address.
	  if (context->object->Administrate(
		context->object->meta.attributes,
   	        context->access->address,
		context->object->meta.owner.permissions,
		context->object->meta.owner.token) == StatusError)
	    escape("unable to update the object's meta section");
	}

      leave();
    }

  }
}
