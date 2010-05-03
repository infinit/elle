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
// updated       julien quintard   [mon may  3 22:34:00 2010]
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

#include <elle/archive/Archive.hh>

#include <elle/network/Host.hh>
#include <elle/network/Port.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace radix;
  using namespace archive;

  namespace network
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents an address i.e a host and port.
    ///
    class Address:
      public Object<>
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
      Status		Create(const Host&,
			       const Port);

      //
      // interfaces
      //

      // object
      declare(Address, _());
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
