//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/network/Host.hh
//
// created       julien quintard   [fri oct 16 04:10:36 2009]
// updated       julien quintard   [sat feb  6 04:37:18 2010]
//

#ifndef ELLE_NETWORK_HOST_HH
#define ELLE_NETWORK_HOST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>
#include <elle/misc/Misc.hh>
#include <elle/io/IO.hh>

#include <QHostAddress>
#include <QString>

namespace elle
{
  namespace network
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a network host.
    ///
    class Host:
      public Entity,
      public Dumpable
    {
    public:
      //
      // globals
      //
      static Host		Null;

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
      Host();

      //
      // methods
      //
      Status		Create(const Type);
      Status		Create(const String&);

      //
      // interfaces
      //

      // entity
      Embed(Entity, Host);
      Boolean		operator==(const Host&) const;

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Type		type;
      ::QHostAddress	location;
    };

  }
}

#endif
