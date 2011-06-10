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
// updated       julien quintard   [thu may  5 16:15:57 2011]
//

#ifndef ETOILE_COMPONENTS_ETOILE_HH
#define ETOILE_COMPONENTS_ETOILE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/components/Object.hh>

#include <etoile/context/Directory.hh>

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
				     const nucleus::Address&);
      static elle::Status	Add(context::Directory*,
				    const path::Slice&,
				    context::Directory*);
      static elle::Status	Lookup(context::Directory*,
				       const path::Slice&,
				       nucleus::Entry*&);
      static elle::Status	Consult(context::Directory*,
					const nucleus::Index&,
					const nucleus::Size&,
					nucleus::Range<nucleus::Entry>&);
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
