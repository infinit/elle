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
// updated       julien quintard   [wed sep  7 09:32:41 2011]
//

#ifndef NUCLEUS_PROTON_VERSION_HH
#define NUCLEUS_PROTON_VERSION_HH

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
    /// this class represents a version number which are used to distinguish
    /// the versions related to a mutable block.
    ///
    class Version:
      public elle::Object
    {
    public:
      //
      // types
      //
      typedef elle::Natural64		Type;

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
      Version(const Type);

      //
      // methods
      //
      elle::Status		Create(const Type);

      //
      // interfaces
      //

      // object
      declare(Version);
      elle::Boolean		operator==(const Version&) const;
      elle::Boolean		operator<(const Version&) const;
      elle::Boolean		operator>(const Version&) const;
      Version&			operator+=(const elle::Natural32);
      Version			operator+(const Version&) const;

      // dumpable
      elle::Status		Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status		Serialize(elle::Archive&) const;
      elle::Status		Extract(elle::Archive&);

      //
      // attributes
      //
      Type			number;
    };

  }
}

#endif
