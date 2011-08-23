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
// updated       julien quintard   [fri aug 12 13:59:08 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Character.hh>

#include <elle/Elle.hh>

namespace elle
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this value defines the component's name.
  ///
  const Character		Component[] = "elle";

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method initializes the whole Elle library.
  ///
  Status		Elle::Initialize()
  {
    enter();

    // initialize the system module.
    if (System::Initialize() == StatusError)
      escape("unable to initialize the system module");

    // initialize the radix module.
    if (Radix::Initialize() == StatusError)
      escape("unable to initialize the radix module");

    // initialize the standalone module.
    if (Standalone::Initialize() == StatusError)
      escape("unable to initialize the standalone module");

    // initialize the crypto module.
    if (Cryptography::Initialize() == StatusError)
      escape("unable to initialize the cryptographic module");

    // initialize the concurrency module.
    if (Concurrency::Initialize() == StatusError)
      escape("unable to initialize the concurrency module");

    // initialize the network module.
    if (Network::Initialize() == StatusError)
      escape("unable to initialize the network module");

    leave();
  }

  ///
  /// this method cleans the Elle library
  ///
  Status		Elle::Clean()
  {
    enter();

    // clean the network module.
    if (Network::Clean() == StatusError)
      escape("unable to clean the network module");

    // clean the concurrency module.
    if (Concurrency::Clean() == StatusError)
      escape("unable to clean the concurrency module");

    // clean the crypto module.
    if (Cryptography::Clean() == StatusError)
      escape("unable to clean the cryptographic module");

    // clean the standalone module.
    if (Standalone::Clean() == StatusError)
      escape("unable to clean the standalone module");

    // clean the radix module.
    if (Radix::Clean() == StatusError)
      escape("unable to clean the radix module");

    // clean the system module.
    if (System::Clean() == StatusError)
      escape("unable to clean the system module");

    leave();
  }

}
