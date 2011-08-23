//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infi...e/implementations/cirkle/RoutingTable.hh
//
// created       julien quintard   [fri aug 12 15:56:25 2011]
// updated       julien quintard   [fri aug 12 16:00:11 2011]
//

#ifndef HOLE_IMPLEMENTATIONS_CIRKLE_ROUTINGTABLE_HH
#define HOLE_IMPLEMENTATIONS_CIRKLE_ROUTINGTABLE_HH

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
      class RoutingTable:
	public elle::Object
      {
      public:
	//
	// types
	//
	typedef std::map<const Tag, Functionoid*>		Container;
	typedef typename Container::iterator		Iterator;
	typedef typename Container::const_iterator	Scoutor;

      };

    }
  }
}

#endif
