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
// updated       julien quintard   [mon feb 22 23:52:47 2010]
//

#ifndef ELLE_NETWORK_PARAMETERS_HH
#define ELLE_NETWORK_PARAMETERS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/archive/Archive.hh>

namespace elle
{
  using namespace archive;

  namespace network
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// these parameters types are used for network messages. indeed, every
    /// message is declared as being composed of a set of types, represented
    /// by the Parameters class.
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
