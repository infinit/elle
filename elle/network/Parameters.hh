//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Parameters.hh
//
// created       julien quintard   [sun feb 21 19:17:29 2010]
// updated       julien quintard   [mon feb 22 14:38:11 2010]
//

#ifndef ELLE_NETWORK_PARAMETERS_HH
#define ELLE_NETWORK_PARAMETERS_HH

namespace elle
{
  namespace network
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// these variadic templates extend the base class with multiple
    /// parameters.
    ///

    template <typename... T>
    class Parameters
    {
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/network/Parameters.hxx>

#endif
