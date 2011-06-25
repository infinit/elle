//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/proton/Location.hh
//
// created       julien quintard   [wed jun 15 20:11:00 2011]
// updated       julien quintard   [fri jun 24 17:41:18 2011]
//

#ifndef NUCLEUS_PROTON_LOCATION_HH
#define NUCLEUS_PROTON_LOCATION_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Version.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a storage location both in space and time.
    ///
    /// indeed, a location is composed of the address of the object but
    /// also the number of the object's version which it relates to.
    ///
    class Location:
      public elle::Object
    {
    public:
      //
      // constants
      //
      static const Location		Null;

      //
      // constructors & destructors
      //
      Location();

      //
      // methods
      //
      elle::Status	Create(const Address&,
			       const Version& = Version::First);

      //
      // interfaces
      //

      // object
      declare(Location);
      elle::Boolean	operator==(const Location&) const;

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status	Serialize(elle::Archive&) const;
      elle::Status	Extract(elle::Archive&);

      //
      // attributes
      //
      Address		address;
      Version		version;
    };

  }
}

#endif
