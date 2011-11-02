//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [mon jun 20 14:04:21 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/automaton/Rights.hh>
#include <etoile/automaton/Access.hh>

#include <agent/Agent.hh>

namespace etoile
{
  namespace automaton
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method determines the rights the current user has over the
    /// given context.
    ///
    /// note that should have the rights already been determined, the
    /// method returns. therefore, this method can be called successively.
    ///
    elle::Status	Rights::Determine(
			  gear::Object&				context)
    {
      enter();

      // if the rights have already been determined, return.
      if (context.rights.role != nucleus::RoleUnknown)
	leave();

      // determine the rights according to the subject.
      if (agent::Agent::Subject == context.object.owner._subject)
	{
	  //
	  // if the user is the object's owner, retrieve the user's
	  // permissions, token etc. from the object's meta section.
	  //

	  // set the role.
	  context.rights.role = nucleus::RoleOwner;

	  // set the permissions.
	  context.rights.permissions = context.object.meta.owner.permissions;

	  // if a token is present, decrypt it.
	  if (context.object.meta.owner.token != nucleus::Token::Null)
	    {
	      // extract the secret key from the token.
	      if (context.object.meta.owner.token.Extract(
		    agent::Agent::Identity.pair.k,
		    context.rights.key) == elle::StatusError)
		escape("unable to extract the secret key from the token");
	    }

	  // set the record for ease purpose.
	  context.rights.record = context.object.meta.owner._record;
	}
      else
	{
	  //
	  // if the user is not the owner, open the access block and
	  // retrieve the permissions, token etc. from the access record
	  // associated with the subject.
	  //

	  // open the access.
	  if (Access::Open(context) == elle::StatusError)
	    escape("unable to open the access block");

	  // check that the subject is referenced in the access block.
	  if (context.access->Exist(agent::Agent::Subject) == elle::StatusTrue)
	    {
	      //
	      // in this case, the subject is referenced in the ACL, hence
	      // is considered a lord.
	      //
	      nucleus::Record*	record;

	      // retrieve the record associated with this subject.
	      if (context.access->Lookup(agent::Agent::Subject,
					 record) == elle::StatusError)
		escape("unable to retrieve the access record");

	      // set the role.
	      context.rights.role = nucleus::RoleLord;

	      // set the permissions according to the access record.
	      context.rights.permissions = record->permissions;

	      // if a token is present, decrypt it.
	      if (record->token != nucleus::Token::Null)
		{
		  // extract the secret key from the token.
		  if (record->token.Extract(
			agent::Agent::Identity.pair.k,
			context.rights.key) == elle::StatusError)
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

	      // set the role.
	      context.rights.role = nucleus::RoleVassal;

	      // set the permissions.
	      context.rights.permissions = nucleus::PermissionNone;
	    }
	}

      leave();
    }

    ///
    /// this method is triggered whenever the user's rights need to be
    /// recomputed.
    ///
    elle::Status	Rights::Recompute(
			  gear::Object&				context)
    {
      enter();

      // reset the role in order to make sure the Determine() method
      // will carry one.
      context.rights.role = nucleus::RoleUnknown;

      // call Determine().
      if (Rights::Determine(context) == elle::StatusError)
	escape("unable to determine the rights");

      leave();
    }

    ///
    /// this method is triggered whenever the user's permissions have
    /// been changed.
    ///
    /// note however that the token, key etc. are not being changed.
    ///
    elle::Status	Rights::Update(
			  gear::Object&				context,
			  const nucleus::Permissions&		permissions)
    {
      enter();

      // update the permission.
      context.rights.permissions = permissions;

      // also update the record which also include the user's permission.
      context.rights.record.permissions = permissions;

      leave();
    }

  }
}
