//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/network/Location.hh
//
// created       julien quintard   [sat nov 28 12:55:38 2009]
// updated       julien quintard   [sat nov 28 15:22:48 2009]
//

#ifndef ELLE_NETWORK_LOCATION_HH
#define ELLE_NETWORK_LOCATION_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>

#include <elle/network/Address.hh>

namespace elle
{
  namespace network
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a location i.e an address and port.
    ///
    class Location:
      public Entity,
      public Dumpable
    {
    public:
      //
      // constructors & destructors
      //
      Location();
      Location(const Address&,
	       const Natural16);

      //
      // methods
      //
      Status		Create(const Address&,
			       const Natural16);

      //
      // interfaces
      //

      // entity
      Location&		operator=(const Location&);
      Boolean		operator==(const Location&) const;
      Boolean		operator!=(const Location&) const;

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Address		address;
      Natural16		port;
    };

  }
}

#endif
