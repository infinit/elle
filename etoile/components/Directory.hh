//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/components/Directory.hh
//
// created       julien quintard   [fri aug 14 18:57:08 2009]
// updated       julien quintard   [tue apr  6 14:46:58 2010]
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
      static Status	Load(context::Directory*,
			     const hole::Address&);
      static Status	Create(context::Directory*,
			       context::Directory*,
			       const path::Slice&);
      static Status	Exist(context::Directory*,
			      const path::Slice&,
			      Boolean&);
      static Status	Lookup(context::Directory*,
			       const path::Slice&,
			       kernel::Entry*&);
      static Status	Consult(context::Directory*,
				const kernel::Index&,
				const kernel::Size&,
				kernel::Set&);
      static Status	Add(context::Directory*,
			    const path::Slice&,
			    const hole::Address&);
      static Status	Rename(context::Directory*,
			       const path::Slice&,
			       const path::Slice&);
      static Status	Remove(context::Directory*,
			       const path::Slice&);
      static Status	Store(context::Directory*);
      static Status	Destroy(context::Directory*);
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/journal/Journal.hh>

#include <etoile/user/User.hh>

#include <etoile/components/Rights.hh>
#include <etoile/components/Contents.hh>

#endif
