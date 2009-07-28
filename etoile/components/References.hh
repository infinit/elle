//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /data/mycure/repo.../infinit/etoile/components/References.hh
//
// created       julien quintard   [mon jul 27 10:19:21 2009]
// updated       julien quintard   [mon jul 27 15:04:12 2009]
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
      References()
      {
	References::New(*this);
      }

      ~References()
      {
	References::Delete(*this);
      }

      //
      // interfaces
      //

      // object
      static Status	New(References&);
      static Status	Delete(References&);

      /* XXX
      References&	operator=(const References&);
      Boolean		operator==(const References&);
      Boolean		operator!=(const References&);
      */

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
