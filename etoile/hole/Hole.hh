//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/hole/Hole.hh
//
// created       julien quintard   [sun aug  9 16:44:42 2009]
// updated       julien quintard   [fri apr 16 14:08:29 2010]
//

#ifndef ETOILE_HOLE_HOLE_HH
#define ETOILE_HOLE_HOLE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/hole/Address.hh>
#include <etoile/hole/Block.hh>

namespace etoile
{
  namespace hole
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides a wrapping over the storage layer.
    ///
    class Hole
    {
    public:
      //
      // static methods
      //
      static Status	Put(const Address&,
			    const Block*);
      static Status	Get(const Address&,
			    Block*&);
      static Status	Erase(const Address&);
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/hole/Family.hh>

#endif
