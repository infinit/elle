//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/channel/Location.cc
//
// created       julien quintard   [fri oct 16 05:24:44 2009]
// updated       julien quintard   [fri oct 16 05:28:39 2009]
//

#include <elle/channel/Location.hh>

namespace elle
{
  namespace channel
  {

//
// ---------- definitions -----------------------------------------------------
//

    const Location		Null;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the default constructor
    ///
    Location::Location():
      version(Location::VersionNone)
    {
    }

  }
}
