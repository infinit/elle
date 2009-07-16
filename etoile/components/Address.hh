//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /data/mycure/repositories/infinit/core/components/Address.hh
//
// created       julien quintard   [mon feb 16 21:13:00 2009]
// updated       julien quintard   [thu jul 16 11:38:06 2009]
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
  namespace components
  {

//
// ---------- class -----------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Address:
      public Object,
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
      Address()
      {
	Address::New(*this);
      }

      ~Address()
      {
	Address::Delete(*this);
      }

      //
      // methods
      //
      Status		Create(Archivable&);

      //
      // operators
      //
      static Status	New(Address&);
      static Status	Delete(Address&);

      Address&		operator=(const Address&);
      Boolean		operator==(const Address&);
      Boolean		operator!=(const Address&);

      //
      // attributes
      //
      Digest*		digest;

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0);

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);
    };

  }
}

//
// ---------- operators -------------------------------------------------------
//

namespace std
{
  std::ostream&		operator<<(std::ostream&,
				   const etoile::components::Address&);
}

#endif
