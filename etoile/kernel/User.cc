//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/User.cc
//
// created       julien quintard   [thu apr 29 19:56:29 2010]
// updated       julien quintard   [fri may 28 17:45:15 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/kernel/User.hh>

namespace etoile
{
  namespace kernel
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// the component identifier.
    ///
    const elle::String		User::Name = "User";

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// this method initializes the user.
    ///
    User::User():
      PublicKeyBlock::PublicKeyBlock(hole::ComponentUser)
    {
    }

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
