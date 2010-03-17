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
// updated       julien quintard   [tue mar 16 23:45:08 2010]
//

#ifndef ELLE_NETWORK_INPUTS_HXX
#define ELLE_NETWORK_INPUTS_HXX

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
    inline Arguments<G, T...>	Inputs(T&...			objects)
    {
      return (Arguments<G, T...>(objects...));
    }

  }
}

#endif
