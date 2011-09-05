//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/depot/Depot.hh
//
// created       julien quintard   [tue sep  1 01:08:05 2009]
// updated       julien quintard   [sun sep  4 14:01:24 2011]
//

#ifndef ETOILE_DEPOT_DEPOT_HH
#define ETOILE_DEPOT_DEPOT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

namespace etoile
{
  ///
  /// this namespace contains everything related to the storage layer
  /// abstraction especially through the communication with the Hole
  /// component.
  ///
  namespace depot
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class abstracts the storage layer.
    ///
    class Depot
    {
    public:
      //
      // static methods
      //
      static elle::Status	Initialize();
      static elle::Status	Clean();

      //
      // methods
      //
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
