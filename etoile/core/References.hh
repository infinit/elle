//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/References.hh
//
// created       julien quintard   [mon jul 27 10:19:21 2009]
// updated       julien quintard   [wed jul 29 17:03:04 2009]
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
    class References:
      public ContentHashBlock
    {
    public:
      //
      // structures
      //

      ///
      /// this structure defines a references entry.
      ///
      struct		Entry
      {
	Address		address;
	SecretKey	key;
	Natural64	size;
      };

      //
      // constants
      //
      static const String		Class;

      //
      // methods
      //

      // XXX

      //
      // constructors & destructors
      //
      References();
      ~References();

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

      std::list<Entry*>	blocks;
    };

  }
}

#endif
