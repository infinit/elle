#include <etoile/automaton/Rights.hh>
#include <etoile/automaton/Access.hh>
#include <etoile/gear/Object.hh>
#include <etoile/gear/Group.hh>

#include <nucleus/neutron/Role.hh>
#include <nucleus/neutron/Token.hh>
#include <nucleus/neutron/Record.hh>
#include <nucleus/neutron/Permissions.hh>
#include <nucleus/neutron/Access.hh>

#include <agent/Agent.hh>

#include <elle/log.hh>

ELLE_LOG_TRACE_COMPONENT("infinit.etoile.automaton.Rights");

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
      ELLE_LOG_TRACE_SCOPE("determine the user's rights over the "
                           "object context");

      if (context.rights.role != nucleus::neutron::RoleUnknown)
        {
          ELLE_LOG_TRACE("rights have already been determined")

          return elle::Status::Ok;
        }

      // determine the rights according to the subject.
      if (agent::Agent::Subject == context.object.owner_subject())
        {
          //
          // if the user is the object's owner, retrieve the user's
          // permissions, token etc. from the object's meta section.
          //

          ELLE_LOG_TRACE_SCOPE("the user is the object owner");

          // set the role.
          context.rights.role = nucleus::neutron::RoleOwner;

          // set the permissions.
          context.rights.permissions = context.object.owner_permissions();

          // if a token is present, decrypt it.
          if (context.object.owner_token() != nucleus::neutron::Token::Null)
            {
              // extract the secret key from the token.
              if (context.object.owner_token().Extract(
                    agent::Agent::Identity.pair.k,
                    context.rights.key) == elle::Status::Error)
                escape("unable to extract the secret key from the token");
            }

          // set the record for ease purpose.
          context.rights.record = context.object.owner_record();
        }
      else
        {
          //
          // if the user is not the owner, open the access block and
          // retrieve the permissions, token etc. from the access record
          // associated with the subject.
          //

          ELLE_LOG_TRACE_SCOPE("the user is _not_ the object owner");

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

              ELLE_LOG_TRACE_SCOPE("the object reference an Access block");

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
            }
          else
            {
              //
              // the subject seems to be a vassal of some sort i.e either
              // referenced by a Group or referenced nowhere but possessing
              // a token.
              //

              ELLE_LOG_TRACE_SCOPE("the object does not reference an Access block");

              // set the role.
              context.rights.role = nucleus::neutron::RoleVassal;

              // set the permissions.
              context.rights.permissions = nucleus::neutron::PermissionNone;
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

    elle::Status
    Rights::Determine(gear::Group& context)
    {
      ELLE_LOG_TRACE_SCOPE("determine the user's rights over the group context");

      if (context.rights.role != nucleus::neutron::RoleUnknown)
        {
          ELLE_LOG_TRACE("rights have already been determined")

          return elle::Status::Ok;
        }

      // determine the rights according to the subject.
      if (agent::Agent::Subject == context.group->owner_subject())
        {
          //
          // if the user is the group's owner, retrieve the user's
          // token.
          //

          ELLE_LOG_TRACE_SCOPE("the user is the group owner");

          // set the role.
          context.rights.role = nucleus::neutron::RoleOwner;
        }
      else
        {
          //
          // if the user is not the owner, leave the role as unknown.
          //

          ELLE_LOG_TRACE_SCOPE("the user is _not_ the group owner");

          context.rights.role = nucleus::neutron::RoleUnknown;
        }

      return (elle::Status::Ok);
    }

  }
}
