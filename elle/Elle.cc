#include <elle/system/platform.hh>

#include <elle/cryptography/Cryptography.hh>

#include <elle/Elle.hh>
#include <elle/Exception.hh>

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

    return Status::Ok;
  }

}
