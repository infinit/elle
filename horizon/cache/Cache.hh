//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit (c)
//
// file          /home/mycure/infinit/pig/cache/Cache.hh
//
// created       julien quintard   [sat aug  1 01:10:19 2009]
// updated       julien quintard   [tue aug  4 14:00:12 2009]
//

#ifndef PIG_CACHE_CACHE_HH
#define PIG_CACHE_CACHE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <etoile/Etoile.hh>

#include <map>

using namespace elle::core;
using namespace elle::misc;
using namespace elle::crypto;

using namespace etoile::core;

namespace pig
{
  namespace cache
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Cache
    {
    public:
      //
      // enumerations
      //
      enum Type
	{
	  TypePermanent = 1,
	  TypeTemporary = 2
	};

      //
      // structures
      //
      struct Item
      {
	Type		type;
	Address		address;
      };

      //
      // types
      //
      typedef std::map<String, Item*>		Container;
      typedef Container::iterator		Iterator;

      //
      // static methods
      //
      static Status	Search(const String&,
			       Address* = NULL);
      static Status	Put(const String&,
			    const Address&,
			    const Type& = TypeTemporary);
      static Status	Get(const String&,
			    Address&);
      static Status	Dump();

      //
      // static attributes
      //
      static Container	container;
    };

  }
}

#endif
