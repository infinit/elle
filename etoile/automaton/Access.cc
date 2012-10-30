#include <elle/log.hh>

#include <etoile/automaton/Access.hh>
#include <etoile/automaton/Rights.hh>
#include <etoile/depot/Depot.hh>
#include <etoile/gear/Object.hh>

#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Revision.hh>
#include <nucleus/proton/State.hh>
#include <nucleus/neutron/Access.hh>
#include <nucleus/neutron/Permissions.hh>
#include <nucleus/neutron/Subject.hh>
#include <nucleus/neutron/Record.hh>
#include <nucleus/neutron/Index.hh>
#include <nucleus/neutron/Size.hh>
#include <nucleus/neutron/Range.hh>
#include <nucleus/neutron/Token.hh>
#include <nucleus/neutron/Object.hh>

#include <hole/Hole.hh>
#include <agent/Agent.hh>

ELLE_LOG_COMPONENT("infinit.etoile.automaton.Access");

namespace etoile
{
  namespace automaton
  {

    ///
    /// this method opens the access block by loading it if necessary i.e
    /// if the object references such a block.
    ///
    /// if not such block is referenced, an empty access is author .
    ///
    elle::Status        Access::Open(
                          gear::Object&                         context)
    {
      ELLE_TRACE_FUNCTION(context);

      // check if the access has already been opened.
      if (context.access != nullptr)
        return elle::Status::Ok;

      // if an access block is referenced in the object.
      if (context.object->access() != nucleus::proton::Address::null())
        {
          ELLE_TRACE("the Object references an Access block at '%s'",
                     context.object->access());

          // retrieve the access block.
          try
            {
              // XXX[the context should make use of unique_ptr instead
              //     of releasing here.]
              context.access =
                depot::Depot::pull_access(context.object->access()).release();
            }
          catch (std::runtime_error& e)
            {
              escape("unable to retrieve the access block: %s", e.what());
            }
        }
      else
        {
          ELLE_TRACE("the Object does _not_ reference an Access block: "
                     "allocate one");

          context.access =
            new nucleus::neutron::Access(nucleus::proton::Network(Infinit::Network),
                                         agent::Agent::Identity.pair.K);
        }

      return elle::Status::Ok;
    }

    ///
    /// this method adds a record to the access block, granting access
    /// to the given subject.
    ///
    elle::Status        Access::Grant(
                          gear::Object&                         context,
                          const nucleus::neutron::Subject& subject,
                          const nucleus::neutron::Permissions& permissions)
    {
      ELLE_TRACE_FUNCTION(context, subject, permissions);

      // determine the rights over the object.
      if (Rights::Determine(context) == elle::Status::Error)
        escape("unable to determine the rights");

      // verify that the user can modify the accesses.
      if (context.rights.role != nucleus::neutron::Object::RoleOwner)
        escape("the user does not seem to have the permission to modify "
               "the access permissions");

      // update the accesses depending on the subject.
      if (subject == context.object->owner_subject())
        {
          //
          // in this case, the subject represents the object's owner.
          //

          ELLE_TRACE("the target subject is the Object owner");

          // update the permissions.
          if (context.object->Administrate(
                context.object->attributes(),
                permissions) == elle::Status::Error)
            escape("unable to administrate the object");
        }
      else
        {
          //
          // otherwise, the subject is a lord being a user or a group.
          //

          ELLE_TRACE("the target subject is _not_ the Object owner");

          // open the access block.
          if (Access::Open(context) == elle::Status::Error)
            escape("unable to open the access block");

          // look in the access object.
          if (context.access->Exist(subject) == elle::Status::True)
            {
              ELLE_TRACE("the target subject exists in the Access block");

              // update the access block according to the given permissions.
              if (permissions == nucleus::neutron::permissions::none)
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

                  ELLE_TRACE("the target subject has no permission");

                  // remove the record.
                  if (Access::Revoke(context, subject) == elle::Status::Error)
                    escape("unable to revoke the subject's accesses");
                }
              else
                {
                  //
                  // in this case, the access record can be updated.
                  //

                  ELLE_TRACE("the target subject has some permissions over "
                             "the Object");

                  switch (subject.type())
                    {
                    case nucleus::neutron::Subject::TypeUser:
                      {
                        // XXX[remove try/catch later]
                        try
                          {
                            ELLE_TRACE("update the Access user record")
                              {
                                // Create a token based on the fact that
                                // the object does or does not have content.
                                //
                                // If there is no data, set the token as null,
                                // otherwise, compute it base on the key which
                                // has been extracted from the owner's token
                                // i.e rights.key.
                                nucleus::neutron::Token token =
                                  context.object->contents() ==
                                  nucleus::proton::Address::null() ?
                                  nucleus::neutron::Token::null() :
                                  nucleus::neutron::Token(context.rights.key,
                                                          subject.user());

                                // update the record.
                                if (context.access->Update(
                                      subject,
                                      permissions,
                                      token) == elle::Status::Error)
                                  escape("unable to update the access");
                              }
                          }
                        catch (std::exception const& e)
                          {
                            escape("%s", e.what());
                          }
                        break;
                      }
                    case nucleus::neutron::Subject::TypeGroup:
                      {
                        std::unique_ptr<nucleus::neutron::Group> group;

                        // XXX[remove try/catch later]
                        try
                          {
                            ELLE_TRACE("fetch the Group block from the "
                                       "storage layer")
                              {
                                // XXX[the context should make use of unique_ptr instead
                                //     of releasing here.]
                                group =
                                  depot::Depot::pull_group(
                                    subject.group(),
                                    nucleus::proton::Revision::Last);
                              }

                            ELLE_TRACE("update the Access group record")
                              {
                                nucleus::neutron::Token token =
                                  context.object->contents() ==
                                  nucleus::proton::Address::null() ?
                                  nucleus::neutron::Token::null() :
                                  nucleus::neutron::Token(context.rights.key,
                                                          group->pass_K());

                                // update the record.
                                if (context.access->Update(
                                      subject,
                                      permissions,
                                      token) == elle::Status::Error)
                                  escape("unable to update the access");
                              }
                          }
                        catch (std::exception const& e)
                          {
                            escape("%s", e.what());
                          }

                        break;
                      }
                    default:
                      {
                        escape("invalid subject type '%u'", subject.type());
                      }
                    }
                }
            }
          else
            {
              std::unique_ptr<nucleus::neutron::Record> record;

              ELLE_TRACE("the target subject is _not_ present in the "
                         "Access block");

              switch (subject.type())
                {
                case nucleus::neutron::Subject::TypeUser:
                  {
                    // XXX[remove try/catch later]
                    try
                      {
                        ELLE_TRACE("generate a new user record")
                          {
                            nucleus::neutron::Token token =
                              context.object->contents() ==
                              nucleus::proton::Address::null() ?
                              nucleus::neutron::Token::null() :
                              nucleus::neutron::Token(context.rights.key,
                                                      subject.user());

                            // allocate a new record.
                            record.reset(
                              new nucleus::neutron::Record(subject,
                                                           permissions,
                                                           token));
                          }
                      }
                    catch (std::exception const& e)
                      {
                        escape("%s", e.what());
                      }

                    break;
                  }
                case nucleus::neutron::Subject::TypeGroup:
                  {
                    std::unique_ptr<nucleus::neutron::Group> group;

                    // XXX[remove try/catch later]
                    try
                      {
                        ELLE_TRACE("fetch the Group block")
                          {
                            // XXX[the context should make use of unique_ptr instead
                            //     of releasing here.]
                            group =
                              depot::Depot::pull_group(
                                subject.group(),
                                nucleus::proton::Revision::Last);
                          }

                        ELLE_TRACE("generate a new group record")
                          {
                            nucleus::neutron::Token token =
                              context.object->contents() ==
                              nucleus::proton::Address::null() ?
                              nucleus::neutron::Token::null() :
                              nucleus::neutron::Token(context.rights.key,
                                                      group->pass_K());

                            // allocate a new record.
                            record.reset(
                              new nucleus::neutron::Record(subject,
                                                           permissions,
                                                           token));
                          }
                      }
                    catch (std::exception const& e)
                      {
                        escape("%s", e.what());
                      }

                    break;
                  }
                default:
                  {
                    escape("invalid subject type '%u'", subject.type());
                  }
                }

              ELLE_TRACE("add the record to the Access block")
                {
                  // add the record to the access object.
                  if (context.access->Add(record.get()) == elle::Status::Error)
                    escape("unable to add the new record");
                }

              // stop tracking record.
              record.release();
            }

          ELLE_TRACE("administrate the Object so as to mark it as dirty")
            {
              // in any case, the object must be marked as administered i.e dirty
              // so that the meta signature gets re-computed i.e the access
              // fingerprint has probably changed.
              //
              // for more information, please refer to the Object class.
              if (context.object->Administrate(
                    context.object->attributes(),
                    context.object->owner_permissions()) == elle::Status::Error)
                escape("unable to administrate the object");
            }
        }

      ELLE_TRACE("audit the Object")
        {
          // try to audit the object because the current author may have
          // lost its write permission in the process.
          if (Access::Audit(context, subject) == elle::Status::Error)
            escape("unable to audit the object");
        }

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
                          const nucleus::neutron::Subject& subject,
                          nucleus::neutron::Record const*& record)
    {
      ELLE_TRACE_FUNCTION(context, subject);

      // try to make the best of this call.
      if (agent::Agent::Subject == subject)
        {
          //
          // indeed, if the target subject is the current user, determine
          // the user's rights so that this is not to be done later.
          //

          ELLE_TRACE("the target subject is the current user");

          // determine the user's rights on the object.
          if (Rights::Determine(context) == elle::Status::Error)
            escape("unable to determine the user's rights");

          // return the record, if present.
          if (context.rights.role != nucleus::neutron::Object::RoleNone)
            record = context.rights.record;
          else
            record = nullptr;
        }
      else
        {
          //
          // otherwise, proceed normally.
          //

          ELLE_TRACE("the target subject is _not_ the current user");

          // perform the lookup according to the subject.
          if (subject == context.object->owner_subject())
            {
              //
              // if the target subject is the object owner, retrieve the
              // access record from the owner's meta section. note that
              // this record is not part of the object but has been generated
              // automatically when the object was extracted.
              //

              ELLE_TRACE("the target subject is the object owner");

              // return the record.
              record = &context.object->owner_record();

              ELLE_TRACE("Access lookup found record %p from "
                             "context.object.meta.owner", record);
            }
          else
            {
              //
              // if we are dealing with a lord, open the access block
              // in look in it.
              //

              ELLE_TRACE("the target subject is _not_ the object owner: "
                             "look in the Access block");

              // open the access.
              if (Access::Open(context) == elle::Status::Error)
                escape("unable to open the access block");

               // lookup the subject.
              if (context.access->Lookup(subject,
                                         record) == elle::Status::Error)
                escape("unable to lookup in the access object");
            }
        }

      return elle::Status::Ok;
    }

    ///
    /// this method returns a subset---i.e a range---of the access block.
    ///
    elle::Status        Access::Consult(
                          gear::Object& context,
                          const nucleus::neutron::Index& index,
                          const nucleus::neutron::Size& size,
                          nucleus::neutron::Range<
                            nucleus::neutron::Record>& range)
    {
      ELLE_TRACE_FUNCTION(context, index, size);

      // open the access.
      if (Access::Open(context) == elle::Status::Error)
        escape("unable to open the access block");

      // first detach the data from the range.
      if (range.Detach() == elle::Status::Error)
        escape("unable to detach the data from the range");

      // if the index starts with 0, include the owner by creating
      // a record for him.
      if (index == 0)
        {
          // add the record to the range.
          if (range.Add(&context.object->owner_record()) == elle::Status::Error)
            escape("unable to add the owner record");

          // consult the access object by taking care of consulting one
          // record less.
          if (context.access->Consult(index,
                                      size - 1,
                                      range) == elle::Status::Error)
            escape("unable to consult the access");
        }
      else
        {
          // consult the access object by taking care of starting the
          // consultation one index before since the owner record, which
          // is not located in the access block, counts as one record.
          if (context.access->Consult(index - 1,
                                      size,
                                      range) == elle::Status::Error)
            escape("unable to consult the ensemble");
        }

      return elle::Status::Ok;
    }

    ///
    /// this method revokes a subject's access by updating the access records.
    ///
    elle::Status        Access::Revoke(
                          gear::Object&                         context,
                          const nucleus::neutron::Subject& subject)
    {
      ELLE_TRACE_FUNCTION(context, subject);

      // determine the rights over the object.
      if (Rights::Determine(context) == elle::Status::Error)
        escape("unable to determine the rights");

      // verify that the user can modify the accesses.
      if (context.rights.role != nucleus::neutron::Object::RoleOwner)
        escape("the user does not seem to have the permission to revoke "
               "access permissions");

      // update the access block or object according to the subject.
      if (subject == context.object->owner_subject())
        {
          //
          // in this case, the subject represents the object's owner.
          //

          // update the permissions.
          if (context.object->Administrate(
                context.object->attributes(),
                nucleus::neutron::permissions::none) == elle::Status::Error)
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
          if (context.object->Administrate(
                context.object->attributes(),
                context.object->owner_permissions()) == elle::Status::Error)
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
                             nucleus::neutron::permissions::none) == elle::Status::Error)
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
      ELLE_TRACE_FUNCTION(context, key);

      // open the access.
      if (Access::Open(context) == elle::Status::Error)
        escape("unable to open the access");

      // XXX[all this loop was originally part of the Access class. it has
      //     however been extracted because, in order to encrypt the key
      //     for a group, it is necessary to load the group block.
      //     unfortunately, nucleus has no notion of loading/unloading blocks.
      //     one way would have been to provide a function pointer. for example
      //     a Depot class could be introduced in nucleus with virtual methods
      //     for pulling/pushing/wiping blocks.
      //     likewise for Downgrade() below, the loop was in a method in
      //     nucleus::neutron::Access.]

      // Go through the access block's records.
      for (auto record: *context.access)
        {
          // Ignore records which relate to subjects which do not have
          // the read permission; these ones do not have a token.
          if ((record->permissions() & nucleus::neutron::permissions::read) !=
              nucleus::neutron::permissions::read)
            continue;

          switch (record->subject().type())
            {
            case nucleus::neutron::Subject::TypeUser:
              {
                // If the subject is a user, encrypt the key with the
                // user's public key so that she will be the only one
                // capable of decrypting it.

                // XXX[remove try/catch later]
                try
                  {
                    record->token(
                      nucleus::neutron::Token(key, record->subject().user()));
                  }
                catch (std::exception const& e)
                  {
                    escape("%s", e.what());
                  }

                break;
              }
            case nucleus::neutron::Subject::TypeGroup:
              {
                // If the subject is a group, the key is encrypted with the
                // group's public pass. This way, the group members will be
                // able to decrypt it since they have been distributed the
                // private pass.

                std::unique_ptr<nucleus::neutron::Group> group;

                // XXX[remove try/catch later]
                try
                  {
                    // XXX[the context should make use of unique_ptr instead
                    //     of releasing here.]
                    group =
                      depot::Depot::pull_group(
                        record->subject().group(),
                        nucleus::proton::Revision::Last);

                    record->token(
                      nucleus::neutron::Token(key, group->pass_K()));
                  }
                catch (std::exception const& e)
                  {
                    escape("%s", e.what());
                  }

                break;
              }
            default:
              {
                escape("the access block contains unknown entries");
              }
            }

          // Set the access block as being dirty.
          //
          // Note that this operation is done in the loop. This is because,
          // should the access be empty (because it has been opened but no
          // Acccess block is referenced, hence a new one is created), we
          // do not want the new block to be added to the storage layer,
          // since unreferenced.
          context.access->state(nucleus::proton::StateDirty);
        }

      // then, create a new object's owner token.
      //
      // noteworthy is that the owner's token is always computed even though
      // the owner may not have the permission to read. this is required if the
      // owner wants to grant herself back or anyone else the permission
      // to read.
      nucleus::neutron::Token token(key, context.object->owner_K());

      // update the object with the new owner token.
      //
      // let us recall that the owner token is actually included in the
      // object's data section in order for authors to be able to re-generate
      // it.
      if (context.object->Update(
            context.object->author(),
            context.object->contents(),
            context.object->size(),
            context.object->access(),
            token) == elle::Status::Error)
        escape("unable to update the object");

      // determine the rights over the object.
      if (Rights::Determine(context) == elle::Status::Error)
        escape("unable to determine the rights");

      // finally, if the user has the permission to read, update its rights.
      if ((context.rights.permissions & nucleus::neutron::permissions::read) ==
          nucleus::neutron::permissions::read)
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
      ELLE_TRACE_FUNCTION(context);

      // open the access.
      if (Access::Open(context) == elle::Status::Error)
        escape("unable to open the access");

      // XXX[to change later]
      nucleus::neutron::Size n;
      if (context.access->Capacity(n) == elle::Status::Error)
        escape("unable to retrieve the access capacity");

      ELLE_TRACE("set the Access #%s records with a null token", n);

      // Go through the Access records.
      for (auto record: *context.access)
        {
          // Check if the subject has the proper permissions.
          if ((record->permissions() & nucleus::neutron::permissions::read) !=
              nucleus::neutron::permissions::read)
            continue;

          // Reset the token.
          record->token(nucleus::neutron::Token::null());

          // Set the access block as being dirty.
          //
          // Note that this operation is done in the loop. This is because,
          // should the access be empty (because it has been opened but no
          // Acccess block is referenced, hence a new one is created), we
          // do not want the new block to be added to the storage layer,
          // since unreferenced.
          context.access->state(nucleus::proton::StateDirty);
        }

      ELLE_TRACE("update the Object's owner token to null")
        {
          // also update the owner's token.
          if (context.object->Update(
                context.object->author(),
                context.object->contents(),
                context.object->size(),
                context.object->access(),
                nucleus::neutron::Token::null()) == elle::Status::Error)
            escape("unable to update the object");
        }

      // determine the rights over the object.
      if (Rights::Determine(context) == elle::Status::Error)
        escape("unable to determine the rights");

      // finally, if the user has the permission to read, update its rights.
      if ((context.rights.permissions & nucleus::neutron::permissions::read) ==
          nucleus::neutron::permissions::read)
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
      ELLE_TRACE_FUNCTION(context);

      // if the block is present.
      if (context.object->access() != nucleus::proton::Address::null())
        {
          ELLE_TRACE("record the Access block '%s' for removal",
                     context.object->access())
            context.transcript.wipe(context.object->access());
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
      nucleus::neutron::Size size;

      ELLE_TRACE_FUNCTION(context);

      //
      // first, check if the block has been modified i.e exists and is dirty.
      //
      {
        // if there is no loaded access, then there is nothing to do.
        if (context.access == nullptr)
          return elle::Status::Ok;

        // if the access has not changed, do nothing.
        if (context.access->state() == nucleus::proton::StateClean)
          return elle::Status::Ok;
      }

      ELLE_TRACE("the Access block seems to have been modified");

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
          // revisions, removing the access block would render the previous
          // revision inconsistent.
          //
          // therefore, the object is updated with a null access address.
          //
          // however, should the history functionality not be supported
          // by the network, the access block is marked for deletion.
          //

          ELLE_TRACE("the Access block is empty");

          // XXX: restore history handling
          // does the network support the history?
          // if (depot::hole().descriptor().meta().history() == false)
            {
              // destroy the access block.
              if (Access::Destroy(context) == elle::Status::Error)
                escape("unable to destroy the access block");
            }

          // update the object with the null access address.
          if (context.object->Update(
                context.object->author(),
                context.object->contents(),
                context.object->size(),
                nucleus::proton::Address::null(),
                context.object->owner_token()) == elle::Status::Error)
            escape("unable to update the object");
        }
      else
        {
          //
          // otherwise, compute the address of the new access block and
          // update the object accordingly.
          //
          // note that the previous access block is not removed since
          // objects benefit from the history i.e multiple revisions; unless
          // the history support is not activated for this network.
          //
          ELLE_TRACE("the Access block is _not_ empty");

          // XXX: restore history handling
          // does the network support the history?
          // if (depot::hole().descriptor().meta().history() == false)
            {
              // destroy the access block.
              if (Access::Destroy(context) == elle::Status::Error)
                escape("unable to destroy the access block");
            }

          // bind the access as, since the block has changed, its address
          // is going to be different.
          nucleus::proton::Address address(context.access->bind());

          // set the state as consistent.
          context.access->state(nucleus::proton::StateConsistent);

          // finally, update the object with the new access address.
          if (context.object->Update(
                context.object->author(),
                context.object->contents(),
                context.object->size(),
                address,
                context.object->owner_token()) == elle::Status::Error)
            escape("unable to update the object");

          ELLE_TRACE("record the Access block '%s' for storing", address)
            context.transcript.push(address, context.access);
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
                                      const nucleus::neutron::Subject& subject)
    {
      ELLE_TRACE_FUNCTION(context, subject);

      // depending on the current author's role.
      switch (context.object->author().role)
        {
        case nucleus::neutron::Object::RoleOwner:
          {
            //
            // nothing to do in this case: the owner is changing the
            // access control permissions but he is also the one having
            // performed the latest modification on the object i.e the
            // current author.
            //

            break;
          }
        case nucleus::neutron::Object::RoleLord:
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
                nucleus::neutron::Record const* record;

                // retrieve the access record.
                if (context.access->Lookup(subject,
                                           record) == elle::Status::Error)
                  escape("unable to lookup the access record");

                // check that the subject, also author, still has the
                // write permission.
                //
                // if he has, nothing has to be done.
                if ((record->permissions() &
                     nucleus::neutron::permissions::write) ==
                    nucleus::neutron::permissions::write)
                  break;
              }

            // this point is reached if the subject no longer has the
            // write permission, in which case the object's access
            // control mechanism is regulated.
            if (Access::Regulate(context) == elle::Status::Error)
              escape("unable to regulate the object");

            break;
          }
        case nucleus::neutron::Object::RoleVassal:
          {
            // XXX to implement.
            assert(false && "not supported yet");

            break;
          }
        case nucleus::neutron::Object::RoleUnknown:
        case nucleus::neutron::Object::RoleNone:
        default:
          {
            escape("invalid role '%u'", context.object->author().role);
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
      ELLE_TRACE_FUNCTION(context);

      // update the object with a new author. since the object gets updated,
      // it will be re-signed during the object's sealing process.
      if (context.object->Update(
            nucleus::neutron::Author(),
            context.object->contents(),
            context.object->size(),
            context.object->access(),
            context.object->owner_token()) == elle::Status::Error)
        escape("unable to update the object");

      return elle::Status::Ok;
    }

  }
}
