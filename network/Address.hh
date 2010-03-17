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
// updated       julien quintard   [wed mar 17 16:06:36 2010]
//

#ifndef ELLE_NETWORK_ADDRESS_HH
#define ELLE_NETWORK_ADDRESS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>
#include <elle/archive/Archive.hh>

#include <elle/network/Host.hh>
#include <elle/network/Port.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  namespace network
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents an address i.e a host and port.
    ///
    class Address:
      public Entity,
      public Dumpable, public Archivable
    {
    public:
      //
      // globals
      //
      static const Address		Null;

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
      embed(Entity, Address);
      Boolean		operator==(const Address&) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

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
