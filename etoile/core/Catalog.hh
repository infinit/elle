//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/Catalog.hh
//
// created       julien quintard   [mon jul 27 10:19:21 2009]
// updated       julien quintard   [sat aug  1 15:57:36 2009]
//

#ifndef ETOILE_COMPONENTS_CATALOG_HH
#define ETOILE_COMPONENTS_CATALOG_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/core/ContentHashBlock.hh>

#include <list>

namespace etoile
{
  namespace core
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Catalog:
      public ContentHashBlock
    {
    public:
      //
      // structures
      //

      ///
      /// this structure defines a catalog entry.
      ///
      struct		Entry
      {
	String		name;
	Address		address;
      };

      //
      // methods
      //
      Status		Add(String&,
			    Address&);
      Status		Remove(String&);
      Status		Resolve(String&,
				Address&);
      Status		Search(String&,
			       std::list<Entry*>::iterator* = NULL);

      //
      // constructors & destructors
      //
      ~Catalog();

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0);

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      //
      // attributes
      //

      std::list<Entry*>	entries;
    };

  }
}

#endif
