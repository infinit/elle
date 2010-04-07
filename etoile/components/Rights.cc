//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/components/Rights.cc
//
// created       julien quintard   [tue feb  2 16:56:21 2010]
// updated       julien quintard   [tue apr  6 16:06:21 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/components/Rights.hh>

namespace etoile
{
  namespace components
  {

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method determines the rights the current user has over the
    /// given object.
    ///
    Status		Rights::Determine(context::Object*	context)
    {
      user::User*	user;

      enter();

      // if the rights have already been determined, return.
      if (context->rights != NULL)
	leave();

      // load the current user;
      if (user::User::Instance(user) == StatusError)
	escape("unable to load the current user");

      // allocate the structure.
      context->rights = new context::Rights;

      // determine the rights according to the subject.
      if (user->client->agent->K == context->object->owner.K)
	{
	  //
	  // if the user is the object's owner, retrieve the user's
	  // permissions, token etc. from the object's meta section.
	  //

	  // set the role.
	  context->rights->role = kernel::RoleOwner;

	  // set the permissions.
	  context->rights->permissions =
	    context->object->meta.owner.permissions;

	  // also copy the token.
	  context->rights->token =
	    context->object->meta.owner.token;

	  // if a token is present, decrypt it.
	  if (context->rights->token != kernel::Token::Null)
	    {
	      Digest		fingerprint;

	      // decrypt the token.
	      if (user->client->agent->Decrypt(
		    context->rights->token,
		    context->rights->key) == StatusError)
		escape("unable to decrypt the token");

	      // compute a fingerprint of the key.
	      if (OneWay::Hash(context->rights->key,
			       fingerprint) == StatusError)
		escape("unable to compute a fingerprint of the key");

	      // verify the key's validity according to the public fingerprint.
	      if (context->object->data.fingerprint != fingerprint)
		escape("the key granted to the user differs from "
		       "the fingerprint");
	    }
	}
      else
	{
	  //
	  // if the user is not the owner, open the access block and
	  // retrieve the permissions, token etc. from the access record
	  // associated with the subject.
	  //

	  // open the access.
	  if (Access::Open(context) == StatusError)
	    escape("unable to open the access");

	  // test if the user is present in the access block.
	  if (context->access->Exist(user->client->subject) == StatusTrue)
	    {
	      kernel::Record*	record;

	      // retrieve the complete access record.
	      if (context->access->Lookup(user->client->subject,
					  record) == StatusError)
		escape("unable to retrieve the access record");

	      // set the role.
	      context->rights->role = kernel::RoleDelegate;

	      // set the permissions
	      context->rights->permissions = record->permissions;

	      // also copy the token.
	      context->rights->token = record->token;

	      // decrypt the token and verify the key, if present.
	      if (context->rights->token != kernel::Token::Null)
		{
		  Digest	fingerprint;

		  // decrypt the token.
		  if (user->client->agent->Decrypt(
                        context->rights->token,
		        context->rights->key) == StatusError)
		    escape("unable to decrypt the token");

		  // compute a fingerprint of the key.
		  if (OneWay::Hash(context->rights->key,
				   fingerprint) == StatusError)
		    escape("unable to compute a fingerprint of the key");

		  // verify the key's validity according to the
		  // public fingerprint.
		  if (context->object->data.fingerprint != fingerprint)
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
	      context->rights->role = kernel::RoleUnknown;
	      context->rights->permissions = kernel::PermissionNone;
	    }
	}

      leave();
    }

  }
}
