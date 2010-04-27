//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/path/Item.hh
//
// created       julien quintard   [fri aug  7 21:41:33 2009]
// updated       julien quintard   [mon apr 26 18:47:38 2010]
//

#ifndef ETOILE_PATH_ITEM_HH
#define ETOILE_PATH_ITEM_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/path/Slice.hh>

#include <etoile/hole/Address.hh>

#include <map>
#include <utility>

namespace etoile
{
  namespace path
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this structure represents an item, either a file or a link.
    /// items store the string and corresponding address but also a
    /// pointer to the parent directory.
    ///
    /// although it would have been cleaner to rely on class inheritance
    /// to prevent file and links item from storing a pointer to a useless
    /// hierarchy variable, it would have been too much burder for no
    /// benefit.
    ///
    struct Item:
      public Meta,
      public Dumpable
    {
    public:
      //
      // types
      //
      typedef std::map<Slice, Item*>		Container;
      typedef std::pair<Slice, Item*>		Value;
      typedef Container::iterator		Iterator;
      typedef Container::const_iterator		Scoutor;

      //
      // constructors & destructors
      //
      Item();
      Item(const Slice&,
	   Item*);
      ~Item();

      //
      // methods
      //
      Status		Resolve(const Slice&,
				Item*&);
      Status		Update(const Slice&,
			       const hole::Address&);
      Status		Destroy();

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0);

      //
      // attributes
      //
      Slice		name;
      hole::Address	address;

      Item*		directory;

      Container*	data;
    };

  }
}

#endif
