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
// updated       julien quintard   [mon jul 18 23:27:40 2011]
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
