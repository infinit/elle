//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/network/Address.hh
//
// created       julien quintard   [sat nov 28 12:55:38 2009]
// updated       julien quintard   [wed feb  3 21:50:22 2010]
//

#ifndef ELLE_NETWORK_ADDRESS_HH
#define ELLE_NETWORK_ADDRESS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>

#include <elle/network/Host.hh>
#include <elle/network/Port.hh>

namespace elle
{
  namespace network
  {

//
// ---------- classes ---------------------------------------------------------
//

// this one is address
// address becomes host

    ///
    /// this class represents an address i.e a host and port.
    ///
    class Address:
      public Entity,
      public Dumpable
    {
    public:
      //
      // constructors & destructors
      //
      Address();

      //
      // methods
      //
      Status		Create(const Host&,
			       const Port);

      //
      // interfaces
      //

      // entity
      Embed(Entity, Address);
      Boolean		operator==(const Address&) const;

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Host		host;
      Port		port;
    };

  }
}

#endif
