//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/automaton/Rights.cc
//
// created       julien quintard   [mon jun 20 14:04:21 2011]
// updated       julien quintard   [wed jun 22 16:25:08 2011]
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

	  /* XXX
	  // open the access.
	  if (Access::Open(gear) == elle::StatusError)
	    escape("unable to open the access");

	  // test if the user is present in the access block.
	  if (gear->access->Exist(user->client->subject) ==
	      elle::StatusTrue)
	    {
	      nucleus::Record*	record;

	      // retrieve the complete access record.
	      if (gear->access->Lookup(user->client->subject,
					  record) == elle::StatusError)
		escape("unable to retrieve the access record");

	      // set the role.
	      gear->rights->role = nucleus::RoleDelegate;

	      // set the record.
	      gear->rights->record = *record;

	      // decrypt the token and verify the key, if present.
	      if (gear->rights->record.token != nucleus::Token::Null)
		{
		  elle::Digest	fingerprint;

		  // extract the secret key from the token.
		  if (gear->rights->record.token.Extract(
                        *user->client->agent,
		        gear->rights->key) == elle::StatusError)
		    escape("unable to extract the secret key from the token");

		  // compute a fingerprint of the key.
		  if (elle::OneWay::Hash(gear->rights->key,
					 fingerprint) == elle::StatusError)
		    escape("unable to compute a fingerprint of the key");

		  // verify the key's validity according to the
		  // public fingerprint.
		  if (gear->object->data.fingerprint != fingerprint)
		    escape("the key granted to the user differs from "
			   "the fingerprint");
		}
	    }
	  else
	    {
	      //
	      // if the user is not present in the access block, she is
	      // at best, a vassal but it is her job to prove it by providing
	      // a voucher.
	      //

	      // therefore, at this point, the user is considered as having no
	      // right over the object.
	      gear->rights->role = nucleus::RoleUnknown;
	      gear->rights->record.permissions = nucleus::PermissionNone;
	    }
	  */
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
