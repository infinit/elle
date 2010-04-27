//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/libraries/elle/network/Parameters.hh
//
// created       julien quintard   [sun feb 21 19:17:29 2010]
// updated       julien quintard   [mon apr 26 19:08:34 2010]
//

#ifndef ELLE_NETWORK_PARAMETERS_HH
#define ELLE_NETWORK_PARAMETERS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Meta.hh>

#include <elle/archive/Archive.hh>

#include <elle/idiom/Open.hh>

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
    /// note that all the methods are thread-safe, cf. Parameters.hxx
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
