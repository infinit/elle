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
// updated       julien quintard   [thu may  5 16:06:12 2011]
//

#ifndef ETOILE_PATH_ITEM_HH
#define ETOILE_PATH_ITEM_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/path/Slice.hh>

#include <elle/idiom/Close.hh>
# include <map>
# include <utility>
#include <elle/idiom/Open.hh>

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
      public elle::Entity
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
      elle::Status	Resolve(const Slice&,
				Item*&);
      elle::Status	Update(const Slice&,
			       const nucleus::Address&);
      elle::Status	Destroy();

      //
      // interfaces
      //

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0);

      //
      // attributes
      //
      Slice		name;
      nucleus::Address	address;

      Item*		directory;

      Container*	data;
    };

  }
}

#endif
