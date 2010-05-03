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
// updated       julien quintard   [mon may  3 22:59:52 2010]
//

#ifndef ETOILE_HOLE_ADDRESS_HH
#define ETOILE_HOLE_ADDRESS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/hole/Family.hh>

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
      public elle::Object<elle::FormatBase64, elle::FormatBase64>
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
			       const elle::Archivable&);

      //
      // attributes
      //
      Family		family;
      elle::Digest*	digest;

      //
      // interfaces
      //

      // object
      declare(Address, _(elle::FormatBase64, elle::FormatBase64));
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
