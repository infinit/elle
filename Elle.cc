//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/Elle.cc
//
// created       julien quintard   [wed mar  3 23:26:52 2010]
// updated       julien quintard   [thu mar  4 11:06:29 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace elle
{

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method initializes the whole Elle library.
  ///
  Status		Elle::Initialize()
  {
    enter();

    // initialize the crypto module.
    if (Cryptography::Initialize() == StatusError)
      escape("unable to initialize the cryptographic module");

    // XXX network -> bridge
    // XXX factory

    leave();
  }

  ///
  /// this method cleans the Elle library
  ///
  Status		Elle::Clean()
  {
    enter();

    // clean the crypto module.
    if (Cryptography::Clean() == StatusError)
      escape("unable to clean the cryptographic module");

    leave();
  }

}
