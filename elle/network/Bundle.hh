//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Bundle.hh
//
// created       julien quintard   [fri jun  3 22:23:13 2011]
// updated       julien quintard   [thu jun  9 17:27:47 2011]
//

#ifndef ELLE_NETWORK_BUNDLE_HH
#define ELLE_NETWORK_BUNDLE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/radix/Status.hh>
#include <elle/radix/Entity.hh>
#include <elle/radix/Arguments.hh>

#include <elle/network/Tag.hh>
#include <elle/network/Message.hh>

namespace elle
{
  using namespace radix;

  namespace network
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a set of arguments associated
    /// with a network tag.
    ///
    /// note that this class is specialized for inputs (const) and ouptuts
    /// (non-const).
    ///
    template <const Tag G,
	      typename... T>
    class Bundle;

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/network/Bundle.hxx>

#endif
