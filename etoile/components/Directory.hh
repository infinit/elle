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
// updated       julien quintard   [mon aug 17 11:51:51 2009]
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

#include <etoile/components/Object.hh>
#include <etoile/components/Catalog.hh>

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
      static Status	Load(context::Object&,
			     const hole::Address&);
      static Status	Create(context::Object&);
      static Status	Destroy(context::Object&);
      static Status	Store(context::Object&);

      static Status	Consult(context::Object&,
				std::list<String>&,
				const Natural64 = core::Catalog::Index::First,
				const Natural64 = core::Catalog::Index::Last);
      static Status	Lookup(context::Object&,
			       const String&,
			       hole::Address&);
      static Status	Add(context::Object&,
			    const String&,
			    const hole::Address&);
      static Status	Remove(context::Object&,
			       const String&);
      static Status	Update(context::Object&,
			       const String&,
			       const hole::Address&);
      static Status	Rename(context::Object&,
			       const String&,
			       const String&);
    };

  }
}

#endif
