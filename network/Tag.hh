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
// updated       julien quintard   [wed mar 31 14:54:04 2010]
//

#ifndef ELLE_NETWORK_CODE_HH
#define ELLE_NETWORK_CODE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>

namespace elle
{
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
    /// this defines that unused tag value.
    ///
    const Tag			TagNone = 0;

  }
}

#endif
