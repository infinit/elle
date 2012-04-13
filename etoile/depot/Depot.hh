//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [tue sep  1 01:08:05 2009]
//

#ifndef ETOILE_DEPOT_DEPOT_HH
#define ETOILE_DEPOT_DEPOT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>
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
      static elle::Status       Initialize();
      static elle::Status       Clean();

      //
      // methods
      //
      static elle::Status       Origin(nucleus::Address&);

      static elle::Status       Push(const nucleus::Address&,
                                     const nucleus::Block&);
      static elle::Status       Pull(const nucleus::Address&,
                                     const nucleus::Version&,
                                     nucleus::Block&);
      static elle::Status       Wipe(const nucleus::Address&);
    };

  }
}

#endif
