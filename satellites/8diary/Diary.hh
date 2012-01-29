//
// ---------- header ----------------------------------------------------------
//
// project       8diary
//
// license       infinit
//
// author        julien quintard   [mon jun 27 22:39:27 2011]
//

#ifndef DIARY_DIARY_HH
#define DIARY_DIARY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <Infinit.hh>
#include <elle/Elle.hh>
#include <lune/Lune.hh>
#include <hole/Hole.hh>
#include <agent/Agent.hh>
#include <etoile/Etoile.hh>

namespace application
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class implements the 8diary application.
  ///
  class Diary
  {
  public:
    //
    // enumerations
    //
    enum Operation
      {
        OperationUnknown = 0,

        OperationRecord,
        OperationReplay,
        OperationDump
      };
  };

}

//
// ---------- includes --------------------------------------------------------
//

#include <applications/8diary/Memoirs.hh>

#if defined(INFINIT_UNIX)
# include <applications/8diary/unix/UNIX.hh>
#elif defined(INFINIT_WINDOWS)
// XXX todo: windows
#else
# error "unsupported platform"
#endif

#endif
