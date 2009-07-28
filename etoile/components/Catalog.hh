//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /data/mycure/repo...ies/infinit/etoile/components/Catalog.hh
//
// created       julien quintard   [mon jul 27 10:19:21 2009]
// updated       julien quintard   [mon jul 27 14:41:28 2009]
//

#ifndef ETOILE_COMPONENTS_CATALOG_HH
#define ETOILE_COMPONENTS_CATALOG_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/components/ContentHashBlock.hh>

#include <list>

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
      // constants
      //
      static const String		Class;

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
      Catalog()
      {
	Catalog::New(*this);
      }

      ~Catalog()
      {
	Catalog::Delete(*this);
      }

      //
      // interfaces
      //

      // object
      static Status	New(Catalog&);
      static Status	Delete(Catalog&);

      /* XXX
      Catalog&		operator=(const Catalog&);
      Boolean		operator==(const Catalog&);
      Boolean		operator!=(const Catalog&);
      */

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
