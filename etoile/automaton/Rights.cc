#include <etoile/automaton/Rights.hh>
#include <etoile/automaton/Access.hh>
#include <etoile/gear/Object.hh>

#include <nucleus/neutron/Role.hh>
#include <nucleus/neutron/Token.hh>
#include <nucleus/neutron/Record.hh>
#include <nucleus/neutron/Permissions.hh>
#include <nucleus/neutron/Access.hh>

#include <agent/Agent.hh>

#include <elle/log.hh>

ELLE_LOG_COMPONENT("etoile.automaton.Rights");

namespace etoile
{
  namespace automaton
  {

    ///
    /// this method determines the rights the current user has over the
    /// given context.
    ///
    /// note that should have the rights already been determined, the
    /// method returns. therefore, this method can be called successively.
    ///
    elle::Status        Rights::Determine(
                          gear::Object&                         context)
    {
      if (context.rights.role != nucleus::neutron::RoleUnknown)
        {
          ELLE_TRACE("Rights have already been determined !")
          return elle::Status::Ok;
        }

      // determine the rights according to the subject.
      if (agent::Agent::Subject == context.object->owner.subject)
        {
          //
          // if the user is the object's owner, retrieve the user's
          // permissions, token etc. from the object's meta section.
          //

          // set the role.
          context.rights.role = nucleus::neutron::RoleOwner;

          // set the permissions.
          context.rights.permissions = context.object->meta.owner.permissions;

          // if a token is present, decrypt it.
          if (context.object->meta.owner.token != nucleus::neutron::Token::Null)
            {
              // extract the secret key from the token.
              if (context.object->meta.owner.token.Extract(
                    agent::Agent::Identity.pair.k,
                    context.rights.key) == elle::Status::Error)
                escape("unable to extract the secret key from the token");
            }

          // set the record for ease purpose. XXX I see what you did there
          context.rights.record = context.object->meta.owner.record;
          ELLE_TRACE("Rights have been determined according to the subject record.");
        }
      else
        {
          //
          // if the user is not the owner, open the access block and
          // retrieve the permissions, token etc. from the access record
          // associated with the subject.
          //

          // open the access.
          if (Access::Open(context) == elle::Status::Error)
            escape("unable to open the access block");

          // check that the subject is referenced in the access block.
          if (context.access->Exist(agent::Agent::Subject) == elle::Status::True)
            {
              //
              // in this case, the subject is referenced in the ACL, hence
              // is considered a lord.
              //
              nucleus::neutron::Record* record;

              // retrieve the record associated with this subject.
              if (context.access->Lookup(agent::Agent::Subject,
                                         record) == elle::Status::Error)
                escape("unable to retrieve the access record");

              // set the role.
              context.rights.role = nucleus::neutron::RoleLord;

              // set the permissions according to the access record.
              context.rights.permissions = record->permissions;

              // if a token is present, decrypt it.
              if (record->token != nucleus::neutron::Token::Null)
                {
                  // extract the secret key from the token.
                  if (record->token.Extract(
                        agent::Agent::Identity.pair.k,
                        context.rights.key) == elle::Status::Error)
                    escape("unable to extract the secret key from the token");
                }

              // finally, set the record for ease purpose.
              context.rights.record = *record;
              ELLE_TRACE("Rights have been determined from its own context (referenced in the access block).");
            }
          else
            {
              //
              // the subject seems to be a vassal of some sort i.e either
              // referenced by a Group or referenced nowhere but possessing
              // a token.
              //

              // set the role.
              context.rights.role = nucleus::neutron::RoleVassal;

              // set the permissions.
              context.rights.permissions = nucleus::neutron::PermissionNone;
              ELLE_TRACE("Rights default to Vassal role and permissions");
            }
        }

      return elle::Status::Ok;
    }

    ///
    /// this method is triggered whenever the user's rights need to be
    /// recomputed.
    ///
    elle::Status        Rights::Recompute(
                          gear::Object&                         context)
    {
      // reset the role in order to make sure the Determine() method
      // will carry one.
      context.rights.role = nucleus::neutron::RoleUnknown;

      // call Determine().
      if (Rights::Determine(context) == elle::Status::Error)
        escape("unable to determine the rights");

      return elle::Status::Ok;
    }

    ///
    /// this method is triggered whenever the user's permissions have
    /// been changed.
    ///
    /// note however that the token, key etc. are not being changed.
    ///
    elle::Status        Rights::Update(
                          gear::Object&                         context,
                          const nucleus::neutron::Permissions& permissions)
    {
      // update the permission.
      context.rights.permissions = permissions;

      // also update the record which also include the user's permission.
      context.rights.record.permissions = permissions;

      return elle::Status::Ok;
    }

    ///
    /// this method checks whether the user has the necessary permissions
    /// and eventually the required role to perform the given operation.
    ///
    elle::Status        Rights::Operate(
                          gear::Object&                         context,
                          const gear::Operation&                operation)
    {
      // depending on the operation.
      switch (operation)
        {
        case gear::OperationUnknown:
          {
            escape("unable to check the rights for a unknown operation");
          }
        case gear::OperationDiscard:
          {
            //
            // nothing to check since discarding does not require
            // special privileges.
            //

            break;
          }
        case gear::OperationStore:
          {
            //
            // in this case, the user must have had the permission to write
            // the object.
            //
            // however, the permission checking process would have been
            // performed once the modifying operation such as Add() for
            // a directory would have been invoked.
            //
            // therefore, no special check is performed here.
            //

            break;
          }
        case gear::OperationDestroy:
          {
            //
            // in this case, the user must be the object's owner in order
            // to destroy it.
            //

            // determine the user's rights on this context.
            if (Rights::Determine(context) == elle::Status::Error)
              escape("unable to determine the rights");

            // check if the current user has the given role.
            if (context.rights.role != nucleus::neutron::RoleOwner)
              escape("the user does not seem to have the permission to "
                     "perform the requested operation");

            break;
          }
        }

      return elle::Status::Ok;
    }

  }
}
