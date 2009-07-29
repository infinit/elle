//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/Address.hh
//
// created       julien quintard   [mon feb 16 21:13:00 2009]
// updated       julien quintard   [wed jul 29 16:44:39 2009]
//

#ifndef ETOILE_COMPONENTS_ADDRESS_HH
#define ETOILE_COMPONENTS_ADDRESS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <ostream>

namespace etoile
{
  namespace core
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
      static const String		Class;

      //
      // constructors & destructors
      //
      Address();
      ~Address();

      //
      // methods
      //
      Status		Create(Archivable&);

      //
      // attributes
      //
      Digest*		digest;

      //
      // interfaces
      //

      // entity
      Address&		operator=(const Address&);
      Boolean		operator==(const Address&);
      Boolean		operator!=(const Address&);

      // dumpable
      Status		Dump(const Natural32 = 0);

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);
    };

  }
}

#endif
