//
// ---------- header ----------------------------------------------------------
//
// project       facade
//
// license       infinit
//
// author        julien quintard   [tue may 31 10:32:37 2011]
//

#ifndef FACADE_WINDOWS_WINDOWS_HH
#define FACADE_WINDOWS_WINDOWS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <Infinit.hh>
#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>
#include <lune/Lune.hh>

namespace facade
{
  ///
  /// this namespace contains all the elements related to the windows facade
  /// i.e interface between the kernel and Infinit.
  ///
  namespace windows
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class contains the general-purpose information related to the
    /// windows facade.
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
