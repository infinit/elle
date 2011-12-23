//
// ---------- header ----------------------------------------------------------
//
// project       8diary
//
// license       infinit
//
// author        julien.quintard   [thu dec 22 15:30:50 2011]
//

#ifndef DIARY_UNIX_UNIX_HH
#define DIARY_UNIX_UNIX_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace application
{
  ///
  /// this namespace contains the UNIX-related stuff for enabling the diary.
  /// recording/replaying.
  ///
  namespace unix
  {

    // nothing

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <applications/8diary/unix/Crux.hh>
#include <applications/8diary/unix/Live.hh>
#include <applications/8diary/unix/Memoirs.hh>
#include <applications/8diary/unix/Record.hh>
#include <applications/8diary/unix/Replay.hh>
#include <applications/8diary/unix/Upcall.hh>

#endif
