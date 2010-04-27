//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/libraries/elle/Elle.cc
//
// created       julien quintard   [wed mar  3 23:26:52 2010]
// updated       julien quintard   [tue apr 27 13:26:26 2010]
//

//
// ---------- includes --------------------------------------------------------
//

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

    // initialize the core module.
    if (Core::Initialize() == StatusError)
      escape("unable to initialize the core module");

    // initialize the system module.
    if (System::Initialize() == StatusError)
      escape("unable to initialize the system module");

    // initialize the crypto module.
    if (Cryptography::Initialize() == StatusError)
      escape("unable to initialize the cryptographic module");

    // initialize the misc module.
    if (Miscellaneous::Initialize() == StatusError)
      escape("unable to initialize the misc module");

    // initialize the factory module.
    if (Factory::Initialize() == StatusError)
      escape("unable to initialize the factory module");

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

    // clean the factory module.
    if (Factory::Clean() == StatusError)
      escape("unable to clean the factory module");

    // clean the misc module.
    if (Miscellaneous::Clean() == StatusError)
      escape("unable to clean the misc module");

    // clean the crypto module.
    if (Cryptography::Clean() == StatusError)
      escape("unable to clean the cryptographic module");

    // clean the system module.
    if (System::Clean() == StatusError)
      escape("unable to clean the system module");

    // clean the core module.
    if (Core::Clean() == StatusError)
      escape("unable to clean the core module");

    leave();
  }

}
