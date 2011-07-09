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
// updated       julien quintard   [fri jul  8 12:15:46 2011]
//

#ifndef ELLE_NETWORK_OUTPUTS_HXX
#define ELLE_NETWORK_OUTPUTS_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Bundle.hh>

namespace elle
{
  namespace network
  {

//
// ---------- functions -------------------------------------------------------
//

    ///
    /// this function generates a Bundle instance.
    ///
    /// this function is being inlined in order to avoid copying the
    /// Bundle instance for nothing.
    ///
    template <const Tag G,
	      typename... T>
    inline
    typename Message<G>::B::Outputs	Outputs(T&...		objects)
    {
      return (typename Message<G>::B::Outputs(objects...));
    }

  }
}

#endif
