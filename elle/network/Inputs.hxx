//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Inputs.hxx
//
// created       julien quintard   [wed feb 24 07:44:04 2010]
// updated       julien quintard   [sun sep  4 12:40:10 2011]
//

#ifndef ELLE_NETWORK_INPUTS_HXX
#define ELLE_NETWORK_INPUTS_HXX

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
    /// this function generates an inputs bundle instance without having
    /// to specify the parameter types.
    ///
    /// this function is being inlined in order to avoid copying the
    /// Bundle instance for nothing.
    ///
    template <const Tag G,
	      typename... T>
    inline
    typename Message<G>::B::Inputs	Inputs(const T&...	objects)
    {
      return (typename Message<G>::B::Inputs(objects...));
    }

  }
}

#endif
