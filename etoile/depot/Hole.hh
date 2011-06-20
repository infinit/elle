//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/depot/Hole.hh
//
// created       julien quintard   [thu may 12 14:43:26 2011]
// updated       julien quintard   [tue jun 14 21:49:10 2011]
//

#ifndef ETOILE_DEPOT_HOLE_HH
#define ETOILE_DEPOT_HOLE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

namespace etoile
{
  namespace depot
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
      static elle::Status	Initialize();
      static elle::Status	Clean();

      static elle::Status	Origin(nucleus::Address&);

      static elle::Status	Push(const nucleus::Address&,
				     const nucleus::Block&);
      static elle::Status	Pull(const nucleus::Address&,
				     const nucleus::Version&,
				     nucleus::Block&);
      static elle::Status	Wipe(const nucleus::Address&);
    };

  }
}

#endif
