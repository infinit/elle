//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/wall/User.cc
//
// created       julien quintard   [thu mar  4 16:27:52 2010]
// updated       julien quintard   [wed mar 10 22:11:50 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/wall/User.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    Status		User::Identify(const PublicKey&		K,
				       const String&		phrase)
    {
      enter();

      printf("User::Identifiy\n");

      context->Dump();

      // XXX

      leave();
    }

    ///
    /// XXX
    ///
    Status		User::Authenticate(const Digest&	digest)
    {
      enter();

      // XXX

      leave();
    }

    ///
    /// XXX
    ///
    Status		User::Connect(const String&		phrase)
    {
      enter();

      // XXX

      leave();
    }

  }
}
