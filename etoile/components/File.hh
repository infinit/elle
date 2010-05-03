//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/components/File.hh
//
// created       julien quintard   [fri aug 14 18:57:08 2009]
// updated       julien quintard   [mon may  3 17:54:57 2010]
//

#ifndef ETOILE_COMPONENTS_ETOILE_HH
#define ETOILE_COMPONENTS_ETOILE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/components/Object.hh>

#include <etoile/context/File.hh>

#include <etoile/hole/Address.hh>

#include <etoile/kernel/Catalog.hh>

namespace etoile
{
  namespace components
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides functionalities for managing directories.
    ///
    class File:
      public Object
    {
    public:
      //
      // static methods
      //
      static elle::Status	Create(context::File*);
      static elle::Status	Load(context::File*,
				     const hole::Address&);
      static elle::Status	Write(context::File*,
				      const kernel::Offset&,
				      const elle::Region&);
      static elle::Status	Read(context::File*,
				     const kernel::Offset&,
				     const kernel::Size&,
				     elle::Region&);
      static elle::Status	Adjust(context::File*,
				       const kernel::Size&);
      static elle::Status	Discard(context::File*);
      static elle::Status	Store(context::File*);
      static elle::Status	Destroy(context::File*);
    };

  }
}

#endif
