//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/hole/Address.hh
//
// created       julien quintard   [mon feb 16 21:13:00 2009]
// updated       julien quintard   [tue mar  1 14:33:18 2011]
//

#ifndef ETOILE_HOLE_ADDRESS_HH
#define ETOILE_HOLE_ADDRESS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/hole/Family.hh>
#include <etoile/hole/Component.hh>
#include <etoile/hole/Universe.hh>

#include <ostream>
#include <sstream>

namespace etoile
{
  namespace hole
  {

//
// ---------- class -----------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Address:
      public elle::Object, elle::Fileable<elle::FormatBase64>
    {
    public:
      //
      // constants
      //
      static const Address	Null;

      //
      // constructors & destructors
      //
      Address();
      Address(const Address&);
      ~Address();

      //
      // methods
      //
      elle::Status	Create(const Family,
			       const Component,
			       const elle::Archivable&);

      //
      // attributes
      //
      // XXX Universe		universe;
      Family		family;
      Component		component;
      elle::Digest*	digest;

      //
      // interfaces
      //

      // object
      declare(Address);
      elle::Boolean	operator==(const Address&) const;
      elle::Boolean	operator<(const Address&) const;

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status	Serialize(elle::Archive&) const;
      elle::Status	Extract(elle::Archive&);
    };

  }
}

#endif
