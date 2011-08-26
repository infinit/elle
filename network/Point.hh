//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Point.hh
//
// created       julien quintard   [sat nov 28 12:55:38 2009]
// updated       julien quintard   [fri aug 26 14:34:28 2011]
//

#ifndef ELLE_NETWORK_POINT_HH
#define ELLE_NETWORK_POINT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>
#include <elle/core/Boolean.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Object.hh>

#include <elle/package/Archive.hh>

#include <elle/network/Host.hh>
#include <elle/network/Port.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace radix;
  using namespace package;

  namespace network
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a network point i.e a host and port.
    ///
    class Point:
      public Object
    {
    public:
      //
      // constants
      //
      static const Point		Null;

      //
      // constructors & destructors
      //
      Point();

      //
      // methods
      //
      Status		Create(const String&);
      Status		Create(const Host&,
			       const Port);

      //
      // interfaces
      //

      // object
      declare(Point);
      Boolean		operator==(const Point&) const;
      Boolean		operator<(const Point&) const;
      Boolean		operator>(const Point&) const;

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
