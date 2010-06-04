//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/hole/Hole.hh
//
// created       julien quintard   [sun aug  9 16:44:42 2009]
// updated       julien quintard   [fri jun  4 14:26:15 2010]
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
      static elle::Status	Put(const elle::String&,
				    const Address&,
				    const Block*);
      static elle::Status	Get(const elle::String&,
				    const Address&,
				    Block*&);
      static elle::Status	Erase(const elle::String&,
				      const Address&);
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/hole/Address.hh>
#include <etoile/hole/Block.hh>
#include <etoile/hole/Component.hh>
#include <etoile/hole/Family.hh>

#endif
