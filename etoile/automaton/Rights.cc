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
      if (agent::Agent::Identity.pair.K == context.object.owner.K)
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

	  // XXX to implement.
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
