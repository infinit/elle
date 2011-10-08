//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [mon jun 20 14:59:09 2011]
//
 
//
// ---------- includes --------------------------------------------------------
//

#include <etoile/automaton/Access.hh>
#include <etoile/automaton/Rights.hh>

#include <etoile/depot/Depot.hh>

#include <agent/Agent.hh>
#include <hole/Hole.hh>

namespace etoile
{
  namespace automaton
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method opens the access block by loading it if necessary i.e
    /// if the object references such a block.
    ///
    /// if not such block is referenced, an empty access is author .
    ///
    elle::Status	Access::Open(
			  gear::Object&				context)
    {
      enter();

      // check if the access has already been opened.
      if (context.access != NULL)
	leave();

      // allocate an access block.
      context.access = new nucleus::Access;

      // if an access block is referenced in the object.
      if (context.object.meta.access != nucleus::Address::Null)
	{
	  // retrieve the access block.
	  if (depot::Depot::Pull(context.object.meta.access,
				 nucleus::Version::Any,
				 *context.access) == elle::StatusError)
	    escape("unable to load the access block");
	}
      else
	{
	  // otherwise the block is left empty.
	}

      leave();
    }

    ///
    /// this method adds a record to the access block, granting access
    /// to the given subject.
    ///
    elle::Status	Access::Grant(
			  gear::Object&				context,
			  const nucleus::Subject&		subject,
			  const nucleus::Permissions&		permissions)
    {
      enter();

      // determine the rights over the object.
      if (Rights::Determine(context) == elle::StatusError)
	escape("unable to determine the rights");

      // verify that the user can modify the accesses.
      if (context.rights.role != nucleus::RoleOwner)
	escape("the user does not seem to have the permission to modify "
	       "the access permissions");

      // update the accesses depending on the subject.
      if (subject == context.object.owner._subject)
	{
	  //
	  // in this case, the subject represents the object's owner.
	  //

	  // update the permissions.
	  if (context.object.Administrate(
                context.object.meta.attributes,
		context.object.meta.access,
		permissions,
		context.object.meta.owner.token) == elle::StatusError)
	    escape("unable to update the object's meta section");
	}
      else
	{
	  //
	  // otherwise, the subject is a lord being a user or a group.
	  //
	  nucleus::Record*	record;

	  // open the access block.
	  if (Access::Open(context) == elle::StatusError)
	    escape("unable to open the access block");

	  // look in the access object.
	  if (context.access->Exist(subject) == elle::StatusTrue)
	    {
	      // retrieve the record.
	      if (context.access->Lookup(subject, record) == elle::StatusError)
		escape("unable to retrieve the subject's record");

	      // update the record with the new permissions.
	      if (record->Update(subject,
				 permissions,
				 context.rights.key) == elle::StatusError)
		escape("unable to update the record");
	    }
	  else
	    {
	      enterx(instance(record));

	      // allocate a new record.
	      record = new nucleus::Record;

	      // create the new record.
	      if (record->Update(subject,
				 permissions,
				 context.rights.key) == elle::StatusError)
		escape("unable to create the new record");

	      // add the record to the access object.
	      if (context.access->Add(record) == elle::StatusError)
		escape("unable to add the new record");

	      // stop tracking record.
	      waive(record);

	      // release.
	      release();
	    }
	}

      // is the target subject the user i.e the object owner in this case.
      if (agent::Agent::Subject == subject)
	{
	  // update the context rights.
	  if (Rights::Update(context, permissions) == elle::StatusError)
	    escape("unable to update the rigths");
	}

      // set the context's state.
      context.state = gear::Context::StateModified;

      leave();
    }

    ///
    /// this method looks for the given subject in the accesses and
    /// return the associated permissions.
    ///
    elle::Status	Access::Lookup(
			  gear::Object&				context,
			  const nucleus::Subject&		subject,
			  nucleus::Record*&			record)
    {
      enter();

      // try to make the best of this call.
      if (agent::Agent::Subject == subject)
	{
	  //
	  // indeed, if the target subject is the current user, determine
	  // the user's rights so that this is not to be done later.
	  //

	  // determine the user's rights on the object.
	  if (Rights::Determine(context) == elle::StatusError)
	    escape("unable to determine the user's rights");

	  // return the record.
	  record = &context.rights.record;
	}
      else
	{
	  //
	  // otherwise, proceed normally.
	  //

	  // perform the lookup according to the subject.
	  if (subject == context.object.owner._subject)
	    {
	      //
	      // if the target subject is the object owner, retrieve the
	      // access record from the owner's meta section. note that
	      // this record is not part of the object but has been generated
	      // automatically when the object was extracted.
	      //

	      // return the record.
	      record = &context.object.meta.owner._record;
	    }
	  else
	    {
	      //
	      // if we are dealing with a lord, open the access block
	      // in look in it.
	      //

	      // open the access.
	      if (Access::Open(context) == elle::StatusError)
		escape("unable to open the access block");

	      // lookup the subject.
	      if (context.access->Lookup(subject,
					 record) == elle::StatusError)
		escape("unable to lookup in the access object");
	    }
	}

      leave();
    }

    ///
    /// this method returns a subset---i.e a range---of the access block.
    ///
    elle::Status	Access::Consult(
			  gear::Object&				context,
			  const nucleus::Index&			index,
			  const nucleus::Size&			size,
			  nucleus::Range<nucleus::Record>&	range)
    {
      enter();

      // open the access.
      if (Access::Open(context) == elle::StatusError)
	escape("unable to open the access block");

      // if the index starts with 0, include the owner by creating
      // a record for him.
      if (index == 0)
	{
	  nucleus::Record*	record;
	  nucleus::Subject	subject;

	  enterx(instance(record));

	  // create a subject for the owner.
	  if (subject.Create(context.object.owner.K) == elle::StatusError)
	    escape("unable to create the owner subject");

	  // create the record.
	  record = new nucleus::Record(subject,
				       context.object.meta.owner.permissions,
				       context.object.meta.owner.token);

	  // add the record to the range.
	  if (range.Add(record) == elle::StatusError)
	    escape("unable to add the owner record");

	  // waive.
	  waive(record);

	  // consult the access object by taking care of consulting one
	  // record less.
	  if (context.access->Consult(index,
				      size - 1,
				      range) == elle::StatusError)
	    escape("unable to consult the access object");
	}
      else
	{
	  // consult the access object by taking care of starting the
	  // consultation one index before since the owner record, which
	  // is not located in the access block, counts as one record.
	  if (context.access->Consult(index - 1 ,
				      size,
				      range) == elle::StatusError)
	    escape("unable to consult the access object");
	}

      leave();
    }

    ///
    /// this method revokes a subject's access by updating the access records.
    ///
    elle::Status	Access::Revoke(
			  gear::Object&				context,
			  const nucleus::Subject&		subject)
    {
      enter();

      // determine the rights over the object.
      if (Rights::Determine(context) == elle::StatusError)
	escape("unable to determine the rights");

      // verify that the user can modify the accesses.
      if (context.rights.role != nucleus::RoleOwner)
	escape("the user does not seem to have the permission to revoke "
	       "access permissions");

      // update the access block or object according to the subject.
      if (subject == context.object.owner._subject)
	{
	  //
	  // in this case, the subject represents the object's owner.
	  //

	  // update the permissions.
	  if (context.object.Administrate(
                context.object.meta.attributes,
		context.object.meta.access,
		nucleus::PermissionNone,
		context.object.meta.owner.token) == elle::StatusError)
	    escape("unable to update the object's meta section");
	}
      else
	{
	  //
	  // otherwise, the subject is a lord being a user or a group.
	  //

	  // open the access.
	  if (Access::Open(context) == elle::StatusError)
	    escape("unable to open the access block");

	  // remove the record associated with the given subject.
	  if (context.access->Remove(subject) == elle::StatusTrue)
	    escape("unable to remove the subject's access record");
	}

      // is the target subject the user i.e the object owner in this case.
      if (agent::Agent::Subject == subject)
	{
	  // update the context rights.
	  if (Rights::Update(context,
			     nucleus::PermissionNone) == elle::StatusError)
	    escape("unable to update the rigths");
	}

      // set the context's state.
      context.state = gear::Context::StateModified;

      leave();
    }

    ///
    /// this method is called whenever an object's content has been modified
    /// leading to the generation of a new key.
    ///
    /// this key must then be distributed to every subject authorised to
    /// access the object in reading.
    ///
    /// for that purpose, tokens are generated.
    ///
    elle::Status	Access::Upgrade(
			  gear::Object&				context,
			  const elle::SecretKey&		key)
    {
      enter();

      // open the access.
      if (Access::Open(context) == elle::StatusError)
	escape("unable to open the access");

      // upgrate the access block's records.
      if (context.access->Upgrade(key) == elle::StatusError)
	escape("unable to upgrade the access records");

      // then, update the object's owner token.
      //
      // noteworthy is that the owner's token is always computed even though
      // the owner may not have the permission to read. this is required if the
      // owner wants to grant herself back or anyone else the permission
      // to read.
      if (context.object.meta.owner.token.Update(
            key,
	    context.object.owner.K) == elle::StatusError)
	escape("unable to update the owner's token");

      // recompute the owner record since the token has changed.
      if (context.object.meta.owner._record.Update(
	    context.object.owner._subject,
	    context.object.meta.owner.permissions,
	    context.object.meta.owner.token) == elle::StatusError)
	escape("unable to create the owner access record");

      // update the object's meta section though everything remains
      // the same, including the access address and owner permissions.
      //
      // this is required to make sure the object is marked as dirty.
      if (context.object.Administrate(
	    context.object.meta.attributes,
            context.object.meta.access,
	    context.object.meta.owner.permissions,
	    context.object.meta.owner.token) == elle::StatusError)
	escape("unable to update the object's meta section");

      // determine the rights over the object.
      if (Rights::Determine(context) == elle::StatusError)
	escape("unable to determine the rights");

      // finally, if the user has the permission to read, update its rights.
      if (context.rights.permissions & nucleus::PermissionRead)
	{
	  // recompute the rights with the new key.
	  if (Rights::Recompute(context) == elle::StatusError)
	    escape("unable to recompute the rights");
	}

      // set the context's state.
      context.state = gear::Context::StateModified;

      leave();
    }

    ///
    /// this method downgrades the access tokens i.e in opposition to
    /// upgrading with recompute them.
    ///
    /// this method is called whenever the object references no content.
    /// in such a case, no key needs to be distributed to the lords.
    ///
    /// therefore, the tokens must be reinitialized as null.
    ///
    elle::Status	Access::Downgrade(
			  gear::Object&				context)
    {
      enter();

      // open the access.
      if (Access::Open(context) == elle::StatusError)
	escape("unable to open the access");

      // downgrade the access block's records.
      if (context.access->Downgrade() == elle::StatusError)
	escape("unable to downgrade the access records");

      // determine the rights over the object.
      if (Rights::Determine(context) == elle::StatusError)
	escape("unable to determine the rights");

      // finally, if the user has the permission to read, update its rights.
      if (context.rights.permissions & nucleus::PermissionRead)
	{
	  // recompute the rights.
	  if (Rights::Recompute(context) == elle::StatusError)
	    escape("unable to recompute the rights");
	}

      // set the context's state.
      context.state = gear::Context::StateModified;

      leave();
    }

    ///
    /// this method destroys the access block by recording it in the
    /// transcript.
    ///
    elle::Status	Access::Destroy(
			  gear::Object&				context)
    {
      enter();

      // if the block is present.
      if (context.object.meta.access != nucleus::Address::Null)
	{
	  // mark the access block for removal.
	  if (context.transcript.Wipe(context.object.meta.access) ==
	      elle::StatusError)
	    escape("unable to mark the access block for removal");
	}

      leave();
    }

    ///
    /// this method closes the access.
    ///
    /// should have the access block been modified, the object is updated
    /// accordingly and the necessary blocks are recorded for storing.
    ///
    /// otherwise, nothing has to be done.
    ///
    elle::Status	Access::Close(
			  gear::Object&				context)
    {
      nucleus::Size	size;

      enter();

      //
      // first, check if the block has been modified i.e exists and is dirty.
      //
      {
	// if there is no loaded access, then there is nothing to do.
	if (context.access == NULL)
	  leave();

	// if the access has not changed, do nothing.
	if (context.access->_state == nucleus::StateClean)
	  leave();
      }

      // retrieve the access's size.
      if (context.access->Capacity(size) == elle::StatusError)
	escape("unable to retrieve the access's size");

      //
      // at this point, this access block is known to have been modified.
      //
      // modify the object according to the content of the access block.
      //

      // update the object according to the content.
      if (size == 0)
	{
	  //
	  // if the access became empty after removals, the
	  // object should no longer point to any access block while
	  // the old block should be deleted.
	  //
	  // however, since the object benefits from history i.e several
	  // versions, removing the access block would render the previous
	  // version inconsistent.
	  //
	  // therefore, the object is updated with a null access address.
	  //
	  // however, should the history functionality not be supported
	  // by the network, the access block is marked for deletion.
	  //

	  // does the network support the history?
	  if (hole::Hole::Descriptor.history == false)
	    {
	      // destroy the access block.
	      if (Access::Destroy(context) == elle::StatusError)
		escape("unable to destroy the access block");
	    }

	  // update the object's meta section with the null address.
	  if (context.object.Administrate(
	        context.object.meta.attributes,
		nucleus::Address::Null,
		context.object.meta.owner.permissions,
		context.object.meta.owner.token) == elle::StatusError)
	    escape("unable to update the object's meta section");
	}
      else
	{
	  nucleus::Address	address;

	  //
	  // otherwise, compute the address of the new access block and
	  // update the object accordingly.
	  //
	  // note that the previous access block is not removed since
	  // objects benefit from the history i.e multiple versions; unless
	  // the history support is not activated for this network.
	  //

	  // does the network support the history?
	  if (hole::Hole::Descriptor.history == false)
	    {
	      // destroy the access block.
	      if (Access::Destroy(context) == elle::StatusError)
		escape("unable to destroy the access block");
	    }

	  // bind the access as, since the block has changed, its address
	  // is going to be different.
	  if (context.access->Bind(address) == elle::StatusError)
	    escape("unable to bind the access");

	  // set the state as consistent.
	  context.access->_state = nucleus::StateConsistent;

	  // finally, update the object's meta section with the new address.
	  if (context.object.Administrate(
		context.object.meta.attributes,
   	        address,
		context.object.meta.owner.permissions,
		context.object.meta.owner.token) == elle::StatusError)
	    escape("unable to update the object's meta section");

	  // mark the block as needing to be stored.
	  if (context.transcript.Push(address,
				      context.access) == elle::StatusError)
	    escape("unable to record the object for storing");
	}

      leave();
    }

  }
}

