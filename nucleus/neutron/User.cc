//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/neutron/User.cc
//
// created       julien quintard   [thu apr 29 19:56:29 2010]
// updated       julien quintard   [thu may 12 13:00:35 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/neutron/User.hh>

namespace nucleus
{
  namespace neutron
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates the user given the user's public key.
    ///
    elle::Status	User::Create(const elle::PublicKey&	K)
    {
      enter();

      // create the underlying public key block.
      if (PublicKeyBlock::Create(K) == elle::StatusError)
	escape("unable to create the underlying public key block");

      leave();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the object.
    ///
    embed(User, _());

  }
}
