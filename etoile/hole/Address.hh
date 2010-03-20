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
// updated       julien quintard   [sat mar 20 12:59:39 2010]
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
      Status		Create(const Family,
			       const Archivable&);
      Status		Identify(String&) const;

      //
      // attributes
      //
      Family		family;
      Digest*		digest;

      //
      // interfaces
      //

      // entity
      declare(Entity, Address);
      Boolean		operator==(const Address&) const;

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);
    };


//
// ---------- operators -------------------------------------------------------
//

    elle::Boolean	operator<(const etoile::hole::Address&,
				  const etoile::hole::Address&);

  }
}

#endif
