//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infi.../implementations/cirkle/Neighbourhood.hh
//
// created       julien quintard   [thu aug 25 10:56:32 2011]
// updated       julien quintard   [fri aug 26 13:53:10 2011]
//

#ifndef HOLE_IMPLEMENTATIONS_CIRKLE_NEIGHBOURHOOD_HH
#define HOLE_IMPLEMENTATIONS_CIRKLE_NEIGHBOURHOOD_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <hole/Label.hh>
#include <hole/implementations/cirkle/Neighbour.hh>

namespace hole
{
  namespace implementations
  {
    namespace cirkle
    {

//
// ---------- classes ---------------------------------------------------------
//

      ///
      /// XXX
      ///
      class Neighbourhood:
	public elle::Entity
      {
      public:
	//
	// types
	//
	typedef std::map<const elle::Point, Neighbour*>	Container;
	typedef typename Container::iterator		Iterator;
	typedef typename Container::const_iterator	Scoutor;

	//
	// constructors & destructors
	//
	~Neighbourhood();

	//
	// methods
	//
	elle::Status		Add(const elle::Point&,
				    Neighbour*);
	elle::Status		Exist(const elle::Point&);
	elle::Status		Retrieve(const elle::Point&,
					 Neighbour*&);
	elle::Status		Remove(const elle::Point&);
	elle::Status		Locate(const elle::Point&,
				       Iterator* = NULL);

	//
	// interfaces
	//

	// dumpable
	elle::Status		Dump(const elle::Natural32 = 0) const;

	//
	// attributes
	//
	Container		container;
      };

    }
  }
}

#endif
