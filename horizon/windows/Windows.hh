//
// ---------- header ----------------------------------------------------------
//
// project       horizon
//
// license       infinit
//
// author        julien quintard   [tue may 31 10:32:37 2011]
//

#ifndef HORIZON_WINDOWS_WINDOWS_HH
#define HORIZON_WINDOWS_WINDOWS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <Infinit.hh>
#include <elle/types.hh>
#include <nucleus/Nucleus.hh>
#include <lune/Lune.hh>

namespace horizon
{
  ///
  /// this namespace contains all the elements related to the windows horizon
  /// i.e interface between the kernel and Infinit.
  ///
  namespace windows
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class contains the general-purpose information related to the
    /// windows horizon.
    ///
    class Windows
    {
    public:
      //
      // static methods
      //
      static elle::Status       Initialize();
      static elle::Status       Setup();
      static elle::Status       Clean();

      //
      // static attributes
      //
      // XXX
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

// XXX

#endif
