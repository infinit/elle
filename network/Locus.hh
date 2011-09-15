//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [sat nov 28 12:55:38 2009]
//

#ifndef ELLE_NETWORK_LOCUS_HH
#define ELLE_NETWORK_LOCUS_HH

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
    /// this class represents a network locus i.e a host and port.
    ///
    class Locus:
      public Object
    {
    public:
      //
      // constants
      //
      static const Locus		Null;

      //
      // constructors & destructors
      //
      Locus();

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
      declare(Locus);
      Boolean		operator==(const Locus&) const;
      Boolean		operator<(const Locus&) const;
      Boolean		operator>(const Locus&) const;

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
