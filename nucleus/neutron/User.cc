//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [thu apr 29 19:56:29 2010]
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
    elle::Status        User::Create(const elle::PublicKey&     K)
    {
      // create the underlying public key block.
      if (PublicKeyBlock::Create(K) == elle::Status::Error)
        escape("unable to create the underlying public key block");

      return elle::Status::Ok;
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
