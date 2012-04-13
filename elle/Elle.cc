//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [wed mar  3 23:26:52 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>

#include <elle/types.hh>

namespace elle
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this value defines the component's name.
  ///
  const Character               Component[] = "elle";

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method initializes the whole Elle library.
  ///
  Status                Elle::Initialize()
  {
    // initialize the system module.
    if (System::Initialize() == Status::Error)
      escape("unable to initialize the system module");

    // initialize the radix module.
    if (Radix::Initialize() == Status::Error)
      escape("unable to initialize the radix module");

    // initialize the standalone module.
    if (Standalone::Initialize() == Status::Error)
      escape("unable to initialize the standalone module");

    // initialize the crypto module.
    if (Cryptography::Initialize() == Status::Error)
      escape("unable to initialize the cryptographic module");

    // initialize the concurrency module.
    if (Concurrency::Initialize() == Status::Error)
      escape("unable to initialize the concurrency module");

    // initialize the network module.
    if (Network::Initialize() == Status::Error)
      escape("unable to initialize the network module");

    return Status::Ok;
  }

  ///
  /// this method cleans the Elle library
  ///
  Status                Elle::Clean()
  {
    // clean the network module.
    if (Network::Clean() == Status::Error)
      escape("unable to clean the network module");

    // clean the concurrency module.
    if (Concurrency::Clean() == Status::Error)
      escape("unable to clean the concurrency module");

    // clean the crypto module.
    if (Cryptography::Clean() == Status::Error)
      escape("unable to clean the cryptographic module");

    // clean the standalone module.
    if (Standalone::Clean() == Status::Error)
      escape("unable to clean the standalone module");

    // clean the radix module.
    if (Radix::Clean() == Status::Error)
      escape("unable to clean the radix module");

    // clean the system module.
    if (System::Clean() == Status::Error)
      escape("unable to clean the system module");

    return Status::Ok;
  }

}
