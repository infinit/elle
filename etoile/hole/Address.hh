//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/hole/Address.hh
//
// created       julien quintard   [mon feb 16 21:13:00 2009]
// updated       julien quintard   [fri aug 21 22:10:16 2009]
//

#ifndef ETOILE_HOLE_ADDRESS_HH
#define ETOILE_HOLE_ADDRESS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

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
      public Entity,
      public Dumpable, public Archivable
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
      Status		Create(const Archivable&);
      Status		Identify(String&) const;

      //
      // attributes
      //
      Digest*		digest;

      //
      // interfaces
      //

      // entity
      Address&		operator=(const Address&);
      Boolean		operator==(const Address&) const;
      Boolean		operator!=(const Address&) const;

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);
    };

  }
}

#endif
