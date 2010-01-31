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
// updated       julien quintard   [sat jan 30 16:20:30 2010]
//

#ifndef ETOILE_DEPOT_REPOSITORY_HH
#define ETOILE_DEPOT_REPOSITORY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/configuration/Configuration.hh>

#include <etoile/depot/Record.hh>
#include <etoile/depot/Cell.hh>
#include <etoile/depot/Unit.hh>
#include <etoile/depot/Location.hh>

#include <map>
#include <list>
#include <utility>

namespace etoile
{
  using namespace configuration;

  namespace depot
  {

//
// ---------- forward declarations --------------------------------------------
//

    class Record;
    class Cell;
    class Unit;

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
      static Status	Initialize();
      static Status	Clean();

      static Status	Put(const hole::Address&,
			    hole::Block*);
      static Status	Get(const hole::Address&,
			    hole::Block*&);
      static Status	Evict(const Location,
			      const Natural32);
      static Status	Discard(const hole::Address&);

      static Status	Dump(const Natural32 = 0);

      //
      // static attributes
      //
      static core::Time*		Delays[hole::Families];

      static Data::Container		Container;

      struct				Cache
      {
	static Natural64		Size;
	static Natural64&		Capacity;

	static Access::Container	Queue;
      };

      struct				Reserve
      {
	static String&			Path;

	static Natural64		Size;
	static Natural64&		Capacity;

	static Access::Container	Queue;
      };
    };

  }
}

#endif
