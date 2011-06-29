//
// ---------- header ----------------------------------------------------------
//
// project       8diary
//
// license       infinit
//
// file          /home/mycure/infinit/applications/8diary/Diary.hh
//
// created       julien quintard   [mon jun 27 22:39:27 2011]
// updated       julien quintard   [mon jun 27 22:39:40 2011]
//

#ifndef DIARY_DIARY_HH
#define DIARY_DIARY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <Infinit.hh>
#include <elle/Elle.hh>
#include <lune/Lune.hh>
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
	OperationReplay
      };
  };

}

#endif
