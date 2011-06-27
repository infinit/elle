//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit
//
// file          /home/mycure/infinit/pig/Diary.hh
//
// created       julien quintard   [sun jun 26 22:32:34 2011]
// updated       julien quintard   [mon jun 27 07:34:35 2011]
//

#ifndef PIG_DIARY_HH
#define PIG_DIARY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace pig
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// XXX
  ///
  class Diary
  {
  public:
    //
    // enumerations
    //
    enum Mode
      {
	ModeNone,
	ModeRecord,
	ModeReplay
      };

    //
    // constructors & destructors
    //
    Diary();

    //
    // methods
    //
    elle::Status	Create(const Mode,
			       const elle::String&);

    //
    // attributes
    //
    Mode		mode;
    elle::String	path;
  };

}

#endif
