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
// updated       julien quintard   [wed jan 27 21:31:56 2010]
//

#ifndef ETOILE_DEPOT_REPOSITORY_HH
#define ETOILE_DEPOT_REPOSITORY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/depot/Record.hh>
#include <etoile/depot/Location.hh>

#include <map>
#include <utility>

namespace etoile
{
  namespace depot
  {

//
// ---------- forward declarations --------------------------------------------
//

    class Record;

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
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
	typedef std::pair<hole::Address, Record*>	Value;
	typedef std::map<hole::Address, Record*>	Container;
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
			    hole::Block*,
			    const Natural32);
      static Status	Get(const hole::Address&,
			    hole::Block*&);
      static Status	Evict(const Location,
			      const Natural32);
      static Status	Discard(const hole::Address&);

      static Status	Dump(const Natural32 = 0);

      //
      // static attributes
      //
      static core::Time*		delay[hole::Families];

      static Data::Container		data;

      struct				Cache
      {
	static const Natural64		size;
	static const Natural64		capacity;

	static Access::Container	access;
      };

      struct				Reserve
      {
	static const Natural64		size;
	static const Natural64		capacity;

	static Access::Container	access;
      };
    };

  }
}

#endif
