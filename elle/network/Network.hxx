//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Network.hxx
//
// created       julien quintard   [wed feb  3 16:05:34 2010]
// updated       julien quintard   [tue jul  5 09:27:12 2011]
//

#ifndef ELLE_NETWORK_NETWORK_HXX
#define ELLE_NETWORK_NETWORK_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

namespace elle
{
  namespace network
  {

//
// ---------- network ---------------------------------------------------------
//

    ///
    /// this method registers a callback associated with a set of types so that
    /// whenever a message is received, these types are extracted and the
    /// callback is triggered.
    ///
    template <const Tag G>
    Status		Network::Register(
			  const Callback<
			    typename Trait::Constant<
			      typename Message<G>::P
			      >::Type
			    >&					callback)
    {
      enter();

      // register the callback through the registrar.
      if (Network::Bureau.Register(G, callback) == StatusError)
	escape("unable to register the callback to the registrar");

      leave();
    }

  }
}

#endif
