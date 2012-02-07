//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [fri sep 16 14:00:34 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/proton/Proton.hh>

#include <hole/Hole.hh>

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
      ;
 
      // initialize the address.
      if (Address::Initialize() == elle::StatusError)
        escape("unable to initialize the address");

      return elle::StatusOk;
    }

    ///
    /// this method cleans the proton module.
    ///
    elle::Status        Proton::Clean()
    {
      ;

      // clean the address.
      if (Address::Clean() == elle::StatusError)
        escape("unable to clean the address");

      return elle::StatusOk;
    }

  }
}
