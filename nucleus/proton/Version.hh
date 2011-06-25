//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/proton/Version.hh
//
// created       julien quintard   [fri aug 14 16:24:48 2009]
// updated       julien quintard   [fri jun 24 17:53:56 2011]
//

#ifndef NUCLEUS_NEUTRON_VERSION_HH
#define NUCLEUS_NEUTRON_VERSION_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Version:
      public elle::Object
    {
    public:
      //
      // constants
      //
      static const Version		First;
      static const Version		Last;

      static const Version		Any;

      //
      // constructors & destructors
      //
      Version();
      Version(const elle::Natural64);

      //
      // interfaces
      //

      // object
      declare(Version);
      elle::Boolean	operator==(const Version&) const;
      elle::Boolean	operator<(const Version&) const;
      elle::Boolean	operator>(const Version&) const;
      Version&		operator+=(const elle::Natural32);
      Version		operator+(const Version&) const;

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status	Serialize(elle::Archive&) const;
      elle::Status	Extract(elle::Archive&);

      //
      // attributes
      //
      elle::Natural64	number;
    };

  }
}

#endif
