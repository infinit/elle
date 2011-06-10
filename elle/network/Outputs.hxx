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
// updated       julien quintard   [tue jun  7 06:59:11 2011]
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
    Bundle<G, Parameters<T...> >	Outputs(T&...		objects)
    {
      return (Bundle<G, Parameters<T...> >(objects...));
    }

  }
}

#endif
