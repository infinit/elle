//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit
//
// file          /home/mycure/infinit/pig/diary/Live.hh
//
// created       julien quintard   [fri jul  1 11:39:35 2011]
// updated       julien quintard   [fri jul  1 13:23:37 2011]
//

#ifndef PIG_DIARY_LIVE_HH
#define PIG_DIARY_LIVE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace pig
{
  namespace diary
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Live
    {
    public:
      //
      // classes
      //
      class Item
      {
      public:
	//
	// constructors & destructors
	//
	Item(const elle::Natural64,
	     const elle::Natural64);

	//
	// attributes
	//
	elle::Natural64		passive;
	elle::Natural64		active;
      };

      //
      // types
      //
      typedef std::list<Item*>				Container;
      typedef typename Container::iterator		Iterator;
      typedef typename Container::const_iterator	Scoutor;

      //
      // static methods
      //
      static elle::Status	Initialize();
      static elle::Status	Clean();

      static elle::Status	Add(const elle::Natural64,
				    const elle::Natural64);
      static elle::Status	Retrieve(const elle::Natural64,
					 elle::Natural64&);
      static elle::Status	Remove(const elle::Natural64);

      static elle::Status	Show(const elle::Natural32 = 0);

      //
      // static attributes
      //
      static Container		Items;
    };

  }
}

#endif
