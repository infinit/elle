//
// ---------- header ----------------------------------------------------------
//
// project       diary
//
// license       infinit
//
// author        julien quintard   [thu dec 22 15:30:50 2011]
//

#ifndef DIARY_UNIX_UNIX_HH
#define DIARY_UNIX_UNIX_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace satellite
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

#include <satellites/diary/unix/Crux.hh>
#include <satellites/diary/unix/Live.hh>
#include <satellites/diary/unix/Memoirs.hh>
#include <satellites/diary/unix/Record.hh>
#include <satellites/diary/unix/Replay.hh>
#include <satellites/diary/unix/Upcall.hh>

#endif
