//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       GPL
//
// file          /home/mycure/infinit/infinit/components/Address.hh
//
// created       julien quintard   [mon feb 16 21:13:00 2009]
// updated       julien quintard   [wed mar 11 13:31:06 2009]
//

#ifndef INFINIT_COMPONENTS_ADDRESS_HH
#define INFINIT_COMPONENTS_ADDRESS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <ostream>

namespace infinit
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
				   const infinit::components::Address&);
}

#endif
