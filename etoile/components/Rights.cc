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
// updated       julien quintard   [tue feb  2 19:40:49 2010]
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
    /// XXX
    ///
    Status		Rights::Determine(context::Object*	context)
    {
      Digest		fingerprint;
      Code		token;

      // if the rights have already been determined, return.
      if (context->rights != NULL)
	leave();

      // allocate the structure.
      context->rights = new context::Rights;

      // open the access.
      if (Access::Open(context) == StatusError)
	escape("unable to open the access");

      // get the permissions and token of the given subject i.e
      // the current user.
      if (Access::Retrieve(context,
			   agent::Agent::Subject,
			   context->rights->permissions,
			   token) == StatusError)
	escape("unable to retrieve the permissions and token");

      // decrypt the token with the user's private key.
      if (agent::Agent::Pair.k.Decrypt(token,
				       context->rights->key) == StatusError)
	escape("unable to decrypt the token");

      // compute a fingerprint of the key.
      if (OneWay::Hash(context->rights->key, fingerprint) == StatusError)
	escape("unable to compute a fingerprint of the key");

      // verify the key's validity according to the public fingerprint.
      if (context->object->data.fingerprint != fingerprint)
	escape("the key granted to the user differs from the fingerprint");

      leave();
    }

  }
}
