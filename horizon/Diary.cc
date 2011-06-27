//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit
//
// file          /home/mycure/infinit/pig/Diary.cc
//
// created       julien quintard   [sun jun 26 22:48:13 2011]
// updated       julien quintard   [sun jun 26 23:10:16 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <pig/Diary.hh>

namespace pig
{

//
// ---------- constructors & destructors --------------------------------------
//

  ///
  /// default constructor.
  ///
  Diary::Diary():
    mode(Diary::ModeNone)
  {
  }

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method creates the diary.
  ///
  elle::Status		Diary::Create(const Mode		mode,
				      const elle::String&	path)
  {
    enter();

    // set the attributes.
    this->mode = mode;
    this->path = path;

    leave();
  }

}
