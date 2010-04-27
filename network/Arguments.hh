//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/libraries/elle/network/Arguments.hh
//
// created       julien quintard   [wed feb 24 08:03:32 2010]
// updated       julien quintard   [mon apr 26 19:09:19 2010]
//

#ifndef ELLE_NETWORK_ARGUMENTS_HH
#define ELLE_NETWORK_ARGUMENTS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Meta.hh>

#include <elle/network/Tag.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  namespace network
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class wraps a set of arguments.
    ///
    /// note that references are used in order to avoid copies.
    ///
    template <const Tag G, typename... T>
    class Arguments
    {
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/network/Arguments.hxx>

#endif
