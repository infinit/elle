//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Outputs.hxx
//
// created       julien quintard   [wed feb 24 07:44:04 2010]
// updated       julien quintard   [sun may  2 20:29:24 2010]
//

#ifndef ELLE_NETWORK_OUTPUTS_HXX
#define ELLE_NETWORK_OUTPUTS_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Arguments.hh>

namespace elle
{
  namespace network
  {

//
// ---------- functions -------------------------------------------------------
//

    ///
    /// this function generates an Arguments instance.
    ///
    /// this function is being inlined in order to avoid copying the
    /// Arguments instance for nothing.
    ///
    template <const Tag G, typename... T>
    inline Arguments<G, T...>	Outputs(T&...			objects)
    {
      return (Arguments<G, T...>(objects...));
    }

  }
}

#endif
