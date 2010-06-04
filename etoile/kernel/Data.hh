//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/Data.hh
//
// created       julien quintard   [tue aug  4 06:54:28 2009]
// updated       julien quintard   [fri may 28 17:47:08 2010]
//

#ifndef ETOILE_KERNEL_DATA_HH
#define ETOILE_KERNEL_DATA_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/kernel/Offset.hh>
#include <etoile/kernel/State.hh>

namespace etoile
{
  namespace kernel
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
