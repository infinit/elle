//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit
//
// file          /home/mycure/infinit/pig/diary/Replay.cc
//
// created       julien quintard   [thu jun 30 09:23:09 2011]
// updated       julien quintard   [thu jun 30 10:40:08 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <pig/diary/Replay.hh>
#include <pig/diary/Upcall.hh>

namespace pig
{
  namespace diary
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this variable contains the address of the diary which is
    /// being replayed.
    ///
    Diary*				Replay::Pointer = NULL;

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes FUSE.
    ///
    elle::Status	Replay::Initialize(Diary*		diary)
    {
      enter();

      // set the diary pointer.
      Replay::Pointer = diary;

      leave();
    }

    ///
    /// this method launches the replaying.
    ///
    elle::Status	Replay::Launch()
    {
      Upcall		upcall;
      elle::Natural32	i;

      enter();

      // XXX
      Replay::Pointer->Dump();

      // go through all the diary's upcalls.
      for (i = 0; i < Replay::Pointer->number; i++)
	{
	  // XXX
	  printf("%u\n", i);

	  // retrieve the upcall.
	  if (Replay::Pointer->Read(upcall) == elle::StatusError)
	    escape("unable to read the upcall");
	}

      leave();
    }

    ///
    /// this method cleans FUSE.
    ///
    elle::Status	Replay::Clean()
    {
      enter();

      // reset the diary pointer.
      Replay::Pointer = NULL;

      leave();
    }

  }
}
