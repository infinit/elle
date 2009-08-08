//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/path/Item.hh
//
// created       julien quintard   [fri aug  7 21:41:33 2009]
// updated       julien quintard   [fri aug  7 22:33:59 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <map>

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
    struct Item
    {
    public:
      //
      // types
      //
      typedef std::map<String, Item*>		Container;
      typedef Container::iterator		Iterator;

      //
      // constructors & destructors
      //
      Item();
      ~Item();

      //
      // methods
      //
      Status		Purge();

      Status		Udpate(const Resolution&,
			       const Resolution::Explorer&);

      //
      // attributes
      //
      Address		address;

      Item*		directory;

      Container*	hierarchy;
    };

  }
}
