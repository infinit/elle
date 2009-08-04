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
// updated       julien quintard   [tue aug  4 13:57:18 2009]
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
      // methods
      //

      // XXX

      //
      // constructors & destructors
      //
      ~References();

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

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
