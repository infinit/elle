#include <nucleus/proton/Proton.hh>
#include <nucleus/proton/Address.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes the proton module.
    ///
    elle::Status        Proton::Initialize()
    {
      // initialize the address.
      if (Address::Initialize() == elle::Status::Error)
        escape("unable to initialize the address");

      return elle::Status::Ok;
    }

    ///
    /// this method cleans the proton module.
    ///
    elle::Status        Proton::Clean()
    {
      // clean the address.
      if (Address::Clean() == elle::Status::Error)
        escape("unable to clean the address");

      return elle::Status::Ok;
    }

  }
}
