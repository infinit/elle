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
// updated       julien quintard   [sun mar 21 18:12:11 2010]
//

#ifndef ETOILE_COMPONENTS_ETOILE_HH
#define ETOILE_COMPONENTS_ETOILE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/core/Core.hh>
#include <etoile/context/Context.hh>
#include <etoile/hole/Hole.hh>
#include <etoile/journal/Journal.hh>
#include <etoile/user/User.hh>

#include <etoile/components/Object.hh>
#include <etoile/components/Catalog.hh>
#include <etoile/components/Rights.hh>

namespace etoile
{
  namespace components
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
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
      static Status	Create(context::Directory*);

      static Status	Consult(context::Directory*,
				std::list<String>&,
				const Natural64 = core::Catalog::Index::First,
				const Natural64 = core::Catalog::Index::Last);
      static Status	Lookup(context::Directory*,
			       const String&,
			       hole::Address&);
      static Status	Add(context::Directory*,
			    const String&,
			    const hole::Address&);
      static Status	Remove(context::Directory*,
			       const String&);
      static Status	Update(context::Directory*,
			       const String&,
			       const hole::Address&);
      static Status	Rename(context::Directory*,
			       const String&,
			       const String&);

      static Status	Commit(context::Directory*);
      static Status	Close(context::Directory*);
      static Status	Destroy(context::Directory*);
    };

  }
}

#endif
