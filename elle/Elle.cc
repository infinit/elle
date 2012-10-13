#include <elle/system/platform.hh>

#include <elle/standalone/Report.hh>

#include <elle/cryptography/Cryptography.hh>

#include <elle/radix/Radix.hh>

#include <elle/Elle.hh>

#include <elle/idiom/Open.hh>


namespace elle
{

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method initializes the whole Elle library.
  ///
  Status                Elle::Initialize()
  {
    // initialize the radix module.
    if (radix::Radix::Initialize() == Status::Error)
      escape("unable to initialize the radix module");

    // initialize the crypto module.
    if (cryptography::Cryptography::Initialize() == Status::Error)
      escape("unable to initialize the cryptographic module");

    return Status::Ok;
  }

  ///
  /// this method cleans the Elle library
  ///
  Status                Elle::Clean()
  {
    // clean the crypto module.
    if (cryptography::Cryptography::Clean() == Status::Error)
      escape("unable to clean the cryptographic module");

    // clean the radix module.
    if (radix::Radix::Clean() == Status::Error)
      escape("unable to clean the radix module");

    return Status::Ok;
  }

}
