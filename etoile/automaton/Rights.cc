#include <etoile/automaton/Rights.hh>
#include <etoile/automaton/Access.hh>
#include <etoile/gear/Object.hh>
#include <etoile/gear/Group.hh>

#include <nucleus/neutron/Token.hh>
#include <nucleus/neutron/Record.hh>
#include <nucleus/neutron/Permissions.hh>
#include <nucleus/neutron/Access.hh>
#include <nucleus/neutron/Object.hh>

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

      if (context.rights.role != nucleus::neutron::Object::RoleUnknown)
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
          context.rights.role = nucleus::neutron::Object::RoleOwner;

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

              ELLE_LOG_TRACE_SCOPE("the subject seems to be present in "
                                   "the Access block");

              // retrieve the record associated with this subject.
              if (context.access->Lookup(agent::Agent::Subject,
                                         record) == elle::Status::Error)
                escape("unable to retrieve the access record");

              // set the role.
              context.rights.role = nucleus::neutron::Object::RoleLord;

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

              ELLE_LOG_TRACE_SCOPE("the subject does _not_ seem to be present "
                                   "in the Access block, start looking in the "
                                   "groups");

              /* XXX
              for (auto record: *context.access)
                {
                  if (record->subject.type() == nucleus::neutron::Subject::TypeGroup)
                    {
                      nucleus::neutron::Group group;

                      // Retrieve the group block.
                      if (depot::Depot::Pull(record->subject.group(),
                                             nucleus::neutron::Version::Last,
                                             group) == elle::Status::Error)
                        escape("unable to pull the group block");

                      // Check if the subject is actually the group manager.
                      if (agent::Agent::Subject == group.manager_subject())
                        {
                          ELLE_LOG_TRACE_SCOPE("the subject is the manager of "
                                               "the group '%s'",
                                               record->subject.group());

                          context.rights.role = nucleus::neutron::Object::RoleVassal;
                          context.rights.permissions = record->permissions;

                          // Compute a token with the appropriate information taken
                          // from the group and the access record.
                          nucleus::neutron::Record record(group.manager_subject(),
                                                          record->permissions,
                                                          group.manager_token());
                          context.rights.record = record;

                          // Finally, extract the key from the record.
                          {
XXX
                            group.manager_fellow();
                          if (record->token != nucleus::neutron::Token::Null)
                            {
                              // extract the secret key from the token.
                              if (record->token.Extract(
                                    agent::Agent::Identity.pair.k,
                                    context.rights.key) == elle::Status::Error)
                                escape("unable to extract the secret key from the token");
                            }
                        }
                      else
                        {
                          if (group.ensemble() != nucleus::proton::Address::Null)
                            {
                              nucleus::neutron::Ensemble ensemble;

                              // Retrieve the ensemble which contains the list of
                              // the subjects belonging to the group.
                              if (depot::Depot::Pull(record->subject.group(),
                                                     nucleus::neutron::Version::Last,
                                                     group) == elle::Status::Error)
                                escape("unable to pull the group block");

                              // Look for the user's subject in the ensemble.
                              // XXX[remove the try/catch later]
                              try
                                {

XXX

                              context.rights.role = nucleus::neutron::Object::RoleVassal;
                              context.rights.permissions = record->permissions;
                                }
                              catch (std::exception const& e)
                                {
                                  escape("%s", e.what());
                                }
                            }
                        }
                    }
                }
              */

              // if the system failed at defining the user's role, one can
              // conclude that the user has no role and no permission over
              // the object.
              if (context.rights.role == nucleus::neutron::Object::RoleUnknown)
                {
                  context.rights.role = nucleus::neutron::Object::RoleNone;
                  context.rights.permissions = nucleus::neutron::PermissionNone;
                }
            }
        }

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

      if (context.rights.role != nucleus::neutron::Group::RoleUnknown)
        {
          ELLE_LOG_TRACE("rights have already been determined")

          return elle::Status::Ok;
        }

      // determine the rights according to the subject.
      if (agent::Agent::Subject == context.group->manager_subject())
        {
          //
          // if the user is the group's manager, retrieve the user's
          // token.
          //

          ELLE_LOG_TRACE_SCOPE("the user is the group manager");

          context.rights.role = nucleus::neutron::Group::RoleManager;
        }
      else
        {
          //
          // if the user is not the manager, leave the role as unknown.
          //

          ELLE_LOG_TRACE_SCOPE("the user is _not_ the group manager");

          context.rights.role = nucleus::neutron::Group::RoleNone;
        }

      return (elle::Status::Ok);
    }

  }
}
