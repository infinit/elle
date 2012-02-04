//
// ---------- header ----------------------------------------------------------
//
// project       horizon
//
// license       infinit
//
// author        julien quintard   [thu dec 22 16:09:39 2011]
//

#ifndef HORIZON_HORIZON_HH
#define HORIZON_HORIZON_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/system/Platform.hh>

#include <elle/Elle.hh>

///
/// this namespace contains several implementations related to the
/// operating system as the system call interface changes.
///
namespace horizon
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class contains general-purpose methods for initializing and
  /// cleaning the horizon component.
  ///
  class Horizon
  {
  public:
    //
    // static methods
    //
    static elle::Status         Initialize();
    static elle::Status         Clean();
  };

}

//
// ---------- includes --------------------------------------------------------
//

#if defined(INFINIT_LINUX)
# include <horizon/linux/Linux.hh>
#elif defined(INFINIT_MACOSX)
# include <horizon/macosx/MacOSX.hh>
#elif defined(INFINIT_WINDOWS)
# include <horizon/windows/Windows.hh>
#else
# error "unsupported platform"
#endif

#endif
