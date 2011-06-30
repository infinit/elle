//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit
//
// file          /home/mycure/infinit/pig/diary/Replay.hh
//
// created       julien quintard   [thu jun 30 09:19:53 2011]
// updated       julien quintard   [thu jun 30 09:33:22 2011]
//

#ifndef PIG_DIARY_REPLAY_HH
#define PIG_DIARY_REPLAY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace pig
{
  namespace diary
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// XXX
    ///
    class Diary;

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Replay
    {
    public:
      //
      // static methods
      //
      static elle::Status	Initialize(Diary*);
      static elle::Status	Launch();
      static elle::Status	Clean();

      //
      // static attributes
      //
      static Diary*		Pointer;
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <pig/diary/Diary.hh>

#endif
