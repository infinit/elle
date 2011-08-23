//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Address.hh
//
// created       julien quintard   [sat nov 28 12:55:38 2009]
// updated       julien quintard   [fri aug 12 16:23:46 2011]
//

#ifndef ELLE_NETWORK_ADDRESS_HH
#define ELLE_NETWORK_ADDRESS_HH

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
    /// this class represents an address i.e a host and port.
    ///
    class Address:
      public Object
    {
    public:
      //
      // constants
      //
      static const Address		Null;

      //
      // constructors & destructors
      //
      Address();

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
      declare(Address);
      Boolean		operator==(const Address&) const;
      Boolean		operator<(const Address&) const;
      Boolean		operator>(const Address&) const;

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
