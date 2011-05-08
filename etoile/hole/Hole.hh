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
// updated       julien quintard   [sun may  8 12:26:22 2011]
//

#ifndef ETOILE_HOLE_HOLE_HH
#define ETOILE_HOLE_HOLE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

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
      static elle::Status	Put(const nucleus::Network&,
				    const nucleus::Address&,
				    const nucleus::Block*);
      static elle::Status	Get(const nucleus::Network&,
				    const nucleus::Address&,
				    nucleus::Block*&);
      static elle::Status	Erase(const nucleus::Network&,
				      const nucleus::Address&);
    };

  }
}

#endif
