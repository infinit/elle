//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/XXX/Directory.hh
//
// created       julien quintard   [fri aug 14 18:57:08 2009]
// updated       julien quintard   [thu jun 16 10:44:04 2011]
//

#ifndef ETOILE_AUTOMATON_ETOILE_HH
#define ETOILE_AUTOMATON_ETOILE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/automaton/Object.hh>

#include <etoile/gear/Directory.hh>

namespace etoile
{
  namespace automaton
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
      static elle::Status	Construct(gear::Directory*);
      static elle::Status	Fetch(gear::Directory*,
				      const nucleus::Address&,
				      const);
      static elle::Status	Add(gear::Directory*,
				    const path::Slice&,
				    gear::Directory*);
      static elle::Status	Lookup(gear::Directory*,
				       const path::Slice&,
				       nucleus::Entry*&);
      static elle::Status	Consult(gear::Directory*,
					const nucleus::Index&,
					const nucleus::Size&,
					nucleus::Range<nucleus::Entry>&);
      static elle::Status	Rename(gear::Directory*,
				       const path::Slice&,
				       const path::Slice&);
      static elle::Status	Remove(gear::Directory*,
				       const path::Slice&);
      static elle::Status	Store(gear::Directory*);
      static elle::Status	Discard(gear::Directory*);
      static elle::Status	Destroy(gear::Directory*);
    };

  }
}

#endif
