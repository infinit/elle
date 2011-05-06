//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/neutron/Data.hh
//
// created       julien quintard   [tue aug  4 06:54:28 2009]
// updated       julien quintard   [wed may  4 23:17:44 2011]
//

#ifndef NUCLEUS_NEUTRON_DATA_HH
#define NUCLEUS_NEUTRON_DATA_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <nucleus/neutron/Offset.hh>
#include <nucleus/neutron/State.hh>

namespace nucleus
{
  namespace neutron
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents file data.
    ///
    class Data:
      public elle::Object
    {
    public:
      //
      // constructors & destructors
      //
      Data();

      //
      // methods
      //
      elle::Status	Write(const Offset&,
			      const elle::Region&);
      elle::Status	Read(const Offset&,
			     const Size&,
			     elle::Region&) const;
      elle::Status	Adjust(const Size&);

      elle::Status	Capacity(Size&) const;

      //
      // interfaces
      //

      // object
      declare(Data);

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status	Serialize(elle::Archive&) const;
      elle::Status	Extract(elle::Archive&);

      //
      // attributes
      //
      State		state;

      elle::Region	region;
    };

  }
}

#endif
