//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit (c)
//
// file          /home/mycure/infinit/pig/hole/Hole.hh
//
// created       julien quintard   [sun aug  2 18:20:19 2009]
// updated       julien quintard   [mon aug  3 21:13:18 2009]
//

#ifndef PIG_HOLE_HOLE_HH
#define PIG_HOLE_HOLE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/Etoile.hh>

namespace pig
{
  namespace hole
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Hole
    {
    public:
      //
      // static methods
      //
      static Status	Put(const Address&,
			    const Block&);
      static Status	Get(const Address&,
			    Block&);
      static Status	Destroy(const Address&);
    };

  }
}

#endif
