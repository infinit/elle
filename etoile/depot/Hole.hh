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
// updated       julien quintard   [fri may 13 10:29:47 2011]
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
      // constants
      //
      static const elle::String		Line;

      //
      // static methods
      //
      static elle::Status	Initialize();
      static elle::Status	Clean();

      static elle::Status	Join(const nucleus::Network&);
      static elle::Status	Leave(const nucleus::Network&);

      static elle::Status	Put(const nucleus::Network&,
				    const nucleus::Address&,
				    const nucleus::Block&);
      static elle::Status	Get(const nucleus::Network&,
				    const nucleus::Address&,
				    nucleus::Block&);
      static elle::Status	Erase(const nucleus::Network&,
				      const nucleus::Address&);

      static elle::Status	Error(const elle::Report&);
      static elle::Status	Error(const elle::String&);

      //
      // static attributes
      //
      static elle::Door*		Channel;
    };

  }
}

#endif
