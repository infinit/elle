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
// updated       julien quintard   [fri may  6 13:50:28 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/components/Rights.hh>
#include <etoile/components/Access.hh>

#include <etoile/user/User.hh>

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
    elle::Status	Rights::Determine(context::Object*	context)
    {
      user::User*	user;

      enter();

      // if the rights have already been determined, return.
      if (context->rights != NULL)
	leave();

      // load the current user;
      if (user::User::Instance(user) == elle::StatusError)
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
	  context->rights->role = nucleus::RoleOwner;

	  // set the record.
	  context->rights->record = context->object->meta.owner.record;

	  // if a token is present, decrypt it.
	  if (context->rights->record.token != nucleus::Token::Null)
	    {
	      elle::Digest	fingerprint;

	      // extract the secret key from the token.
	      if (context->rights->record.token.Extract(
                    *user->client->agent,
		    context->rights->key) == elle::StatusError)
		escape("unable to extract the secret key from the token");

	      // compute a fingerprint of the key.
	      if (elle::OneWay::Hash(context->rights->key,
				     fingerprint) == elle::StatusError)
		escape("unable to compute a fingerprint of the key");

	      // verify the key's validity according to the public fingerprint.
	      if (context->object->data.fingerprint != fingerprint)
		{
		  printf("[XXX] this should never happen: tell the user "
			 "that the author is probably malicious\n");

		  escape("the key granted to the user differs from "
			 "the fingerprint");
		}
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
	  if (Access::Open(context) == elle::StatusError)
	    escape("unable to open the access");

	  // test if the user is present in the access block.
	  if (context->access->Exist(user->client->subject) ==
	      elle::StatusTrue)
	    {
	      nucleus::Record*	record;

	      // retrieve the complete access record.
	      if (context->access->Lookup(user->client->subject,
					  record) == elle::StatusError)
		escape("unable to retrieve the access record");

	      // set the role.
	      context->rights->role = nucleus::RoleDelegate;

	      // set the record.
	      context->rights->record = *record;

	      // decrypt the token and verify the key, if present.
	      if (context->rights->record.token != nucleus::Token::Null)
		{
		  elle::Digest	fingerprint;

		  // extract the secret key from the token.
		  if (context->rights->record.token.Extract(
                        *user->client->agent,
		        context->rights->key) == elle::StatusError)
		    escape("unable to extract the secret key from the token");

		  // compute a fingerprint of the key.
		  if (elle::OneWay::Hash(context->rights->key,
					 fingerprint) == elle::StatusError)
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
	      context->rights->role = nucleus::RoleUnknown;
	      context->rights->record.permissions = nucleus::PermissionNone;
	    }
	}

      leave();
    }

    ///
    /// this method updates the user's permissions.
    ///
    elle::Status	Rights::Update(context::Object*		context,
				       const
				         nucleus::Permissions&	permissions)
    {
      enter();

      // reset the permissions.
      context->rights->record.permissions = permissions;

      leave();
    }

  }
}
