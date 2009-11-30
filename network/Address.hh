//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/network/Address.hh
//
// created       julien quintard   [fri oct 16 04:10:36 2009]
// updated       julien quintard   [sat nov 28 15:22:38 2009]
//

#ifndef ELLE_NETWORK_ADDRESS_HH
#define ELLE_NETWORK_ADDRESS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>
#include <elle/misc/Misc.hh>
#include <elle/io/IO.hh>

#include <QHostAddress>

namespace elle
{
  namespace network
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a network address.
    ///
    class Address:
      public Entity,
      public Dumpable
    {
    public:
      //
      // globals
      //
      static Address		Null;

      //
      // enumerations
      //
      enum Type
	{
	  TypeNull,
	  TypeAny,
	  TypeIP,
	  TypeBroadcast
	};

      //
      // constructors & destructors
      //
      Address();

      //
      // methods
      //
      Status		Create(const Type);
      Status		Create(const String&);

      //
      // interfaces
      //

      // entity
      Address&		operator=(const Address&);
      Boolean		operator==(const Address&) const;
      Boolean		operator!=(const Address&) const;

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Type		type;
      ::QHostAddress	address;
    };

  }
}

#endif
