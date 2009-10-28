//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/channel/Location.hh
//
// created       julien quintard   [fri oct 16 04:10:36 2009]
// updated       julien quintard   [fri oct 16 05:31:35 2009]
//

#ifndef ELLE_CHANNEL_LOCATION_HH
#define ELLE_CHANNEL_LOCATION_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>

namespace elle
{
  namespace channel
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a network address.
    ///
    class Location
    {
    public:
      //
      // globals
      //
      static Location			Null;

      //
      // enumerations
      //
      enum Version
	{
	  VersionNone,
	  Version4,
	  Version6
	};

      //
      // constructors & destructors
      //
      Location();

      //
      // attributes
      //
      Version		version;

      union
      {
	Natural32	ipv4;
	Natural8	ipv6[16];
      }			u;
    };

  }
}

#endif
