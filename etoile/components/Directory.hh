//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/components/Directory.hh
//
// created       julien quintard   [fri aug 14 18:57:08 2009]
// updated       julien quintard   [mon may  3 17:55:05 2010]
//

#ifndef ETOILE_COMPONENTS_ETOILE_HH
#define ETOILE_COMPONENTS_ETOILE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/components/Object.hh>

#include <etoile/context/Directory.hh>

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
    class Directory:
      public Object
    {
    public:
      //
      // static methods
      //
      static elle::Status	Create(context::Directory*);
      static elle::Status	Load(context::Directory*,
				     const hole::Address&);
      static elle::Status	Add(context::Directory*,
				    const path::Slice&,
				    context::Directory*);
      static elle::Status	Lookup(context::Directory*,
				       const path::Slice&,
				       kernel::Entry*&);
      static elle::Status	Consult(context::Directory*,
					const kernel::Index&,
					const kernel::Size&,
					kernel::Range<kernel::Entry>&);
      static elle::Status	Rename(context::Directory*,
				       const path::Slice&,
				       const path::Slice&);
      static elle::Status	Remove(context::Directory*,
				       const path::Slice&);
      static elle::Status	Store(context::Directory*);
      static elle::Status	Discard(context::Directory*);
      static elle::Status	Destroy(context::Directory*);
    };

  }
}

#endif
