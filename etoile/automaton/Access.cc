#include <elle/log.hh>

#include <etoile/automaton/Access.hh>
#include <etoile/automaton/Rights.hh>

#include <etoile/depot/Depot.hh>

#include <agent/Agent.hh>
#include <hole/Hole.hh>


namespace etoile
{
  namespace automaton
  {

    ELLE_LOG_TRACE_COMPONENT("etoile.automaton.Access");

    ///
    /// this method opens the access block by loading it if necessary i.e
    /// if the object references such a block.
    ///
    /// if not such block is referenced, an empty access is author .
    ///
    elle::Status        Access::Open(
                          gear::Object&                         context)
    {
      // check if the access has already been opened.
      if (context.access != NULL)
        return elle::Status::Ok;

      // allocate an access block.
      context.access = new nucleus::Access;

      // if an access block is referenced in the object.
      if (context.object.meta.access != nucleus::Address::Null)
        {
          // retrieve the access block.
          if (depot::Depot::Pull(context.object.meta.access,
                                 nucleus::Version::Any,
                                 *context.access) == elle::Status::Error)
            escape("unable to load the access block");
        }
      else
        {
          // otherwise the block is left empty.
        }

      return elle::Status::Ok;
    }

    ///
    /// this method adds a record to the access block, granting access
    /// to the given subject.
    ///
    elle::Status        Access::Grant(
                          gear::Object&                         context,
                          const nucleus::Subject&               subject,
                          const nucleus::Permissions&           permissions)
    {
      // determine the rights over the object.
      if (Rights::Determine(context) == elle::Status::Error)
        escape("unable to determine the rights");

      // verify that the user can modify the accesses.
      if (context.rights.role != nucleus::RoleOwner)
        escape("the user does not seem to have the permission to modify "
               "the access permissions");

      // update the accesses depending on the subject.
      if (subject == context.object.owner.subject)
        {
          //
          // in this case, the subject represents the object's owner.
          //

          // update the permissions.
          if (context.object.Administrate(
                context.object.meta.attributes,
                permissions) == elle::Status::Error)
            escape("unable to administrate the object");
        }
      else
        {
          //
          // otherwise, the subject is a lord being a user or a group.
          //

          // open the access block.
          if (Access::Open(context) == elle::Status::Error)
            escape("unable to open the access block");

          // look in the access object.
          if (context.access->Exist(subject) == elle::Status::True)
            {
              // update the access block according to the given permissions.
              if (permissions == nucleus::PermissionNone)
                {
                  //
                  // in this case, the subject is being removed all his
                  // permissions.
                  //
                  // therefore, rather than keeping an access record with
                  // no permission, the record is removed.
                  //
                  // this way, the access control list is simplified,
                  // potentially reduced to an empty list in which case
                  // the Access block would be removed.
                  //

                  // remove the record.
                  if (context.access->Remove(subject) == elle::Status::Error)
                    escape("unable to remove the access record");
                }
              else
                {
                  //
                  // in this case, the access record can be updated.
                  //

                  // update the record.
                  if (context.access->Update(
                        subject,
                        permissions,
                        context.rights.key) == elle::Status::Error)
                    escape("unable to update the access");
                }
            }
          else
            {
              // allocate a new record.
              std::unique_ptr<nucleus::Record> record{new nucleus::Record};

              // create the new record.
              if (record->Update(subject,
                                 permissions,
                                 context.rights.key) == elle::Status::Error)
                escape("unable to create the new record");

              // add the record to the access object.
              if (context.access->Add(record.get()) == elle::Status::Error)
                escape("unable to add the new record");

              // stop tracking record.
              record.release();
            }

          // in any case, the object must be marked as administered i.e dirty
          // so that the meta signature gets re-computed i.e the access
          // fingerprint has probably changed.
          //
          // for more information, please refer to the Object class.
          if (context.object.Administrate(
                context.object.meta.attributes,
                context.object.meta.owner.permissions) == elle::Status::Error)
            escape("unable to administrate the object");
        }

      // try to audit the object because the current author may have
      // lost its write permission in the process.
      if (Access::Audit(context, subject) == elle::Status::Error)
        escape("unable to audit the object");

      // is the target subject the user i.e the object owner in this case.
      if (agent::Agent::Subject == subject)
        {
          // update the context rights.
          if (Rights::Update(context, permissions) == elle::Status::Error)
            escape("unable to update the rigths");
        }

      // set the context's state.
      context.state = gear::Context::StateModified;

      return elle::Status::Ok;
    }

    ///
    /// this method looks for the given subject in the accesses and
    /// return the associated permissions.
    ///
    elle::Status        Access::Lookup(
                          gear::Object&                         context,
                          const nucleus::Subject&               subject,
                          nucleus::Record*&                     record)
    {
      // try to make the best of this call.
      if (agent::Agent::Subject == subject)
        {
          //
          // indeed, if the target subject is the current user, determine
          // the user's rights so that this is not to be done later.
          //

          // determine the user's rights on the object.
          if (Rights::Determine(context) == elle::Status::Error)
            escape("unable to determine the user's rights");

          // return the record.
          record = &context.rights.record;

          ELLE_LOG_TRACE("Access lookup found record %p from context.rights", record);
        }
      else
        {
          //
          // otherwise, proceed normally.
          //

          // perform the lookup according to the subject.
          if (subject == context.object.owner.subject)
            {
              //
              // if the target subject is the object owner, retrieve the
              // access record from the owner's meta section. note that
              // this record is not part of the object but has been generated
              // automatically when the object was extracted.
              //

              // return the record.
              record = &context.object.meta.owner.record;

              ELLE_LOG_TRACE("Access lookup found record %p from context.object.meta.owner", record);
            }
          else
            {
              //
              // if we are dealing with a lord, open the access block
              // in look in it.
              //

              // open the access.
              if (Access::Open(context) == elle::Status::Error)
                escape("unable to open the access block");


              // lookup the subject.
              if (context.access->Lookup(subject,
                                         record) == elle::Status::Error)
                escape("unable to lookup in the access object");

              ELLE_LOG_TRACE("Access lookup found record %p from context.access", record);
            }
        }

      return elle::Status::Ok;
    }

    ///
    /// this method returns a subset---i.e a range---of the access block.
    ///
    elle::Status        Access::Consult(
                          gear::Object&                         context,
                          const nucleus::Index&                 index,
                          const nucleus::Size&                  size,
                          nucleus::Range<nucleus::Record>&      range)
    {
      // open the access.
      if (Access::Open(context) == elle::Status::Error)
        escape("unable to open the access block");

      // if the index starts with 0, include the owner by creating
      // a record for him.
      if (index == 0)
        {
          // create the record.
          auto record = std::unique_ptr<nucleus::Record>(
              new nucleus::Record(context.object.meta.owner.record)
          );

          // add the record to the range.
          if (range.Add(record.get()) == elle::Status::Error)
            escape("unable to add the owner record");

          // waive.
          record.release();

          // consult the access object by taking care of consulting one
          // record less.
          if (context.access->Consult(index,
                                      size - 1,
                                      range) == elle::Status::Error)
            escape("unable to consult the access object");
        }
      else
        {
          // consult the access object by taking care of starting the
          // consultation one index before since the owner record, which
          // is not located in the access block, counts as one record.
          if (context.access->Consult(index - 1 ,
                                      size,
                                      range) == elle::Status::Error)
            escape("unable to consult the access object");
        }

      return elle::Status::Ok;
    }

    ///
    /// this method revokes a subject's access by updating the access records.
    ///
    elle::Status        Access::Revoke(
                          gear::Object&                         context,
                          const nucleus::Subject&               subject)
    {
      // determine the rights over the object.
      if (Rights::Determine(context) == elle::Status::Error)
        escape("unable to determine the rights");

      // verify that the user can modify the accesses.
      if (context.rights.role != nucleus::RoleOwner)
        escape("the user does not seem to have the permission to revoke "
               "access permissions");

      // update the access block or object according to the subject.
      if (subject == context.object.owner.subject)
        {
          //
          // in this case, the subject represents the object's owner.
          //

          // update the permissions.
          if (context.object.Administrate(
                context.object.meta.attributes,
                nucleus::PermissionNone) == elle::Status::Error)
            escape("unable to administrate the object");
        }
      else
        {
          //
          // otherwise, the subject is a lord being a user or a group.
          //

          // open the access.
          if (Access::Open(context) == elle::Status::Error)
            escape("unable to open the access block");

          // remove the record associated with the given subject.
          if (context.access->Remove(subject) == elle::Status::True)
            escape("unable to remove the subject's access record");

          // the object must be marked as administered i.e dirty so
          // that the meta signature gets re-computed i.e the access
          // fingerprint has probably changed.
          //
          // for more information, please refer to the Object class.
          if (context.object.Administrate(
                context.object.meta.attributes,
                context.object.meta.owner.permissions) == elle::Status::Error)
            escape("unable to administrate the object");
        }

      // try to audit the object because the current author may have
      // lost its write permission in the process.
      if (Access::Audit(context, subject) == elle::Status::Error)
        escape("unable to audit the object");

      // is the target subject the user i.e the object owner in this case.
      if (agent::Agent::Subject == subject)
        {
          // update the context rights.
          if (Rights::Update(context,
                             nucleus::PermissionNone) == elle::Status::Error)
            escape("unable to update the rigths");
        }

      // set the context's state.
      context.state = gear::Context::StateModified;

      return elle::Status::Ok;
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
    elle::Status        Access::Upgrade(
                          gear::Object&                         context,
                          elle::cryptography::SecretKey const&                key)
    {
      nucleus::Token    token;

      // open the access.
      if (Access::Open(context) == elle::Status::Error)
        escape("unable to open the access");

      // upgrate the access block's records.
      if (context.access->Upgrade(key) == elle::Status::Error)
        escape("unable to upgrade the access records");

      // then, create a new object's owner token.
      //
      // noteworthy is that the owner's token is always computed even though
      // the owner may not have the permission to read. this is required if the
      // owner wants to grant herself back or anyone else the permission
      // to read.
      if (token.Update(key, context.object.owner.K) == elle::Status::Error)
        escape("unable to update the owner's token");

      // update the object with the new owner token.
      //
      // let us recall that the owner token is actually included in the
      // object's data section in order for authors to be able to re-generate
      // it.
      if (context.object.Update(
            context.object.author,
            context.object.data.contents,
            context.object.data.size,
            context.object.meta.access,
            token) == elle::Status::Error)
        escape("unable to update the object");

      // determine the rights over the object.
      if (Rights::Determine(context) == elle::Status::Error)
        escape("unable to determine the rights");

      // finally, if the user has the permission to read, update its rights.
      if ((context.rights.permissions & nucleus::PermissionRead) ==
          nucleus::PermissionRead)
        {
          // recompute the rights with the new key.
          if (Rights::Recompute(context) == elle::Status::Error)
            escape("unable to recompute the rights");
        }

      // set the context's state.
      context.state = gear::Context::StateModified;

      return elle::Status::Ok;
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
    elle::Status        Access::Downgrade(
                          gear::Object&                         context)
    {
      // open the access.
      if (Access::Open(context) == elle::Status::Error)
        escape("unable to open the access");

      // downgrade the access block's records.
      if (context.access->Downgrade() == elle::Status::Error)
        escape("unable to downgrade the access records");

      // also update the owner's token.
      if (context.object.Update(
            context.object.author,
            context.object.data.contents,
            context.object.data.size,
            context.object.meta.access,
            nucleus::Token::Null) == elle::Status::Error)
        escape("unable to update the object");

      // determine the rights over the object.
      if (Rights::Determine(context) == elle::Status::Error)
        escape("unable to determine the rights");

      // finally, if the user has the permission to read, update its rights.
      if ((context.rights.permissions & nucleus::PermissionRead) ==
          nucleus::PermissionRead)
        {
          // recompute the rights.
          if (Rights::Recompute(context) == elle::Status::Error)
            escape("unable to recompute the rights");
        }

      // set the context's state.
      context.state = gear::Context::StateModified;

      return elle::Status::Ok;
    }

    ///
    /// this method destroys the access block by recording it in the
    /// transcript.
    ///
    elle::Status        Access::Destroy(
                          gear::Object&                         context)
    {
      // if the block is present.
      if (context.object.meta.access != nucleus::Address::Null)
        {
          // mark the access block for removal.
          if (context.transcript.Wipe(context.object.meta.access) ==
              elle::Status::Error)
            escape("unable to mark the access block for removal");
        }

      return elle::Status::Ok;
    }

    ///
    /// this method closes the access.
    ///
    /// should have the access block been modified, the object is updated
    /// accordingly and the necessary blocks are recorded for storing.
    ///
    /// otherwise, nothing has to be done.
    ///
    elle::Status        Access::Close(
                          gear::Object&                         context)
    {
      nucleus::Size     size;

      //
      // first, check if the block has been modified i.e exists and is dirty.
      //
      {
        // if there is no loaded access, then there is nothing to do.
        if (context.access == NULL)
          return elle::Status::Ok;

        // if the access has not changed, do nothing.
        if (context.access->state == nucleus::StateClean)
          return elle::Status::Ok;
      }

      // retrieve the access's size.
      if (context.access->Capacity(size) == elle::Status::Error)
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
              if (Access::Destroy(context) == elle::Status::Error)
                escape("unable to destroy the access block");
            }

          // update the object with the null access address.
          if (context.object.Update(
                context.object.author,
                context.object.data.contents,
                context.object.data.size,
                nucleus::Address::Null,
                context.object.meta.owner.token) == elle::Status::Error)
            escape("unable to update the object");
        }
      else
        {
          nucleus::Address      address;

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
              if (Access::Destroy(context) == elle::Status::Error)
                escape("unable to destroy the access block");
            }

          // bind the access as, since the block has changed, its address
          // is going to be different.
          if (context.access->Bind(address) == elle::Status::Error)
            escape("unable to bind the access");

          // set the state as consistent.
          context.access->state = nucleus::StateConsistent;

          // finally, update the object with the new access address.
          if (context.object.Update(
                context.object.author,
                context.object.data.contents,
                context.object.data.size,
                address,
                context.object.meta.owner.token) == elle::Status::Error)
            escape("unable to update the object");

          // mark the block as needing to be stored.
          if (context.transcript.Push(address,
                                      context.access) == elle::Status::Error)
            escape("unable to record the object for storing");
        }

      return elle::Status::Ok;
    }

    ///
    /// this method checks if the current object's author is a lord or vassal.
    ///
    /// should it be the case, the system would have to check that the author
    /// did not lose his write permission during the last access control
    /// operation.
    ///
    /// should this occur, the future object retrieval would inevitably
    /// lead the client or server to detect the block as invalid since
    /// the author seems not to have had the right to modify the object.
    ///
    /// note that the _subject_ argument indicates the subject which
    /// access permissions have been altered, the system having to check
    /// that the new subject's permissions do not render the object
    /// inconsistent.
    ///
    elle::Status        Access::Audit(gear::Object&             context,
                                      const nucleus::Subject&   subject)
    {
      // depending on the current author's role.
      switch (context.object.author.role)
        {
        case nucleus::RoleOwner:
          {
            //
            // nothing to do in this case: the owner is changing the
            // access control permissions but he is also the one having
            // performed the latest modification on the object i.e the
            // current author.
            //

            break;
          }
        case nucleus::RoleLord:
          {
            //
            // in this case however, the author is a lord.
            //
            // therefore, the system must make sure the access control
            // operation being performed is not removing the author's right
            // to write the object.
            //
            // to do that, the system tries to locate the access record
            // associated with the subject and verifies that he still has
            // the write permission.

            // open the access block.
            if (Access::Open(context) == elle::Status::Error)
              escape("unable to open the access block");

            // check whether a record exist for the subject as it
            // could very well have been removed.
            if (context.access->Exist(subject) == elle::Status::True)
              {
                nucleus::Record*        record;

                // retrieve the access record.
                if (context.access->Lookup(subject,
                                           record) == elle::Status::Error)
                  escape("unable to lookup the access record");

                // check that the subject, also author, still has the
                // write permission.
                //
                // if he has, nothing has to be done.
                if ((record->permissions & nucleus::PermissionWrite) ==
                    nucleus::PermissionWrite)
                  break;
              }

            // this point is reached if the subject no longer has the
            // write permission, in which case the object's access
            // control mechanism is regulated.
            if (Access::Regulate(context) == elle::Status::Error)
              escape("unable to regulate the object");

            break;
          }
        case nucleus::RoleVassal:
          {
            // XXX to implement.

            break;
          }
        }

      return elle::Status::Ok;
    }

    ///
    /// this method is called whenever the object needs to be regulated
    /// i.e the current author has to be re-generated because the author
    /// just lost the permission to write the object.
    ///
    /// should this occur, the object owner would re-sign the data to
    /// make sure the object consistency is ensured.
    ///
    elle::Status        Access::Regulate(gear::Object&          context)
    {
      nucleus::Author   author;

      // build a new author, representing the object's owner.
      if (author.Create() == elle::Status::Error)
        escape("unable to create the author");

      // update the object with a new author. since the object gets updated,
      // it will be re-signed during the object's sealing process.
      if (context.object.Update(
            author,
            context.object.data.contents,
            context.object.data.size,
            context.object.meta.access,
            context.object.meta.owner.token) == elle::Status::Error)
        escape("unable to update the object");

      return elle::Status::Ok;
    }

  }
}

