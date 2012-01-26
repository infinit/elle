//
// ---------- header ----------------------------------------------------------
//
// project       facade
//
// license       infinit
//
// author        julien quintard   [thu dec 22 16:09:39 2011]
//

#ifndef FACADE_FACADE_HH
#define FACADE_FACADE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/system/Platform.hh>

#include <elle/Elle.hh>

///
/// this namespace contains several implementations related to the
/// operating system as the system call interface changes.
///
namespace facade
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class contains general-purpose methods for initializing and
  /// cleaning the facade component.
  ///
  class Facade
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

#if defined(INFINIT_UNIX)
# include <facade/unix/UNIX.hh>
#elif defined(INFINIT_WIN32)
// XXX todo: windows
#endif

#endif
