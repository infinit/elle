//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Tag.hh
//
// created       julien quintard   [sun nov 29 20:23:37 2009]
// updated       julien quintard   [sun sep  4 13:12:40 2011]
//

#ifndef ELLE_NETWORK_CODE_HH
#define ELLE_NETWORK_CODE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

namespace elle
{
  using namespace core;

  namespace network
  {

//
// ---------- types -----------------------------------------------------------
//

    ///
    /// a tag is included in packets to distinguish their types.
    ///
    /// thus, tags must be used to uniquely identify message types.
    ///
    /// note that the Range can be used to allocate such tags in a safe way.
    ///
    typedef Natural32		Tag;

//
// ---------- tag -------------------------------------------------------------
//

    ///
    /// this defines that unknown tag value.
    ///
    const Tag			TagUnknown = 0;

  }
}

#endif
