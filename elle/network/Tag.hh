#ifndef ELLE_NETWORK_CODE_HH
# define ELLE_NETWORK_CODE_HH

#include <elle/types.hh>

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
    /// thus, tags must be used to uniquely identify message types.
    ///
    /// note that the Range can be used to allocate such tags in a safe way.
    ///
    typedef Natural32           Tag;

//
// ---------- tag -------------------------------------------------------------
//

    ///
    /// this defines that unknown tag value.
    ///
    const Tag                   TagUnknown = 0;

  }
}

#endif
