//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/depot/Repository.hh
//
// created       julien quintard   [tue jan 26 13:24:04 2010]
// updated       julien quintard   [mon may  3 17:39:25 2010]
//

#ifndef ETOILE_DEPOT_REPOSITORY_HH
#define ETOILE_DEPOT_REPOSITORY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <lune/Lune.hh>

#include <etoile/hole/Address.hh>
#include <etoile/hole/Block.hh>
#include <etoile/hole/Family.hh>

#include <etoile/depot/Record.hh>
#include <etoile/depot/Location.hh>

#include <map>
#include <list>
#include <utility>

namespace etoile
{
  namespace depot
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// XXX
    ///
    class Record;

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// the repository holds the data blocks that may be used again. the
    /// repository stores data in records, each record being associated with
    /// an address. then a data can be found in two forms: in main memory
    /// stored in the cache, in a cell structure or on the hard disk, stored
    /// in the reserve, in a unit structure.
    ///
    /// both the cache and the reserve have a least recently used list which
    /// is used to discard blocks whenever space is required.
    ///
    /// XXX \todo explain that the capacity consumption is based on
    ///           Imprint() which is not very precise since an object
    ///           can point to other allocated data!
    class Repository
    {
    public:
      //
      // structures
      //
      struct		Data
      {
	//
	// types
	//
	typedef std::map<hole::Address, Record*>	Container;
	typedef std::pair<hole::Address, Record*>	Value;
	typedef Container::iterator			Iterator;
	typedef Container::const_iterator		Scoutor;
      };

      struct		Access
      {
	//
	// types
	//
	typedef std::list<Record*>			Container;
	typedef Container::iterator			Iterator;
	typedef Container::const_iterator		Scoutor;
      };

      //
      // methods
      //
      static elle::Status	Initialize();
      static elle::Status	Clean();

      static elle::Status	Put(const hole::Address&,
				    hole::Block*);
      static elle::Status	Get(const hole::Address&,
				    hole::Block*&);
      static elle::Status	Evict(const Location,
				      const elle::Natural32);
      static elle::Status	Discard(const hole::Address&);

      static elle::Status	Show(const elle::Natural32 = 0);

      //
      // static attributes
      //
      static elle::Time*		Delays[hole::Families];

      static Data::Container		Container;

      struct				Cache
      {
	static elle::Natural64		Size;
	static elle::Natural64		Capacity;

	static Access::Container	Queue;
      };

      struct				Reserve
      {
	static elle::String		Path;

	static elle::Natural64		Size;
	static elle::Natural64		Capacity;

	static Access::Container	Queue;
      };
    };

  }
}

#endif
