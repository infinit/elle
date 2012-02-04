//
// ---------- header ----------------------------------------------------------
//
// project       horizon
//
// license       infinit
//
// author        julien quintard   [tue jul 26 15:33:54 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <horizon/macosx/ParallelFUker.hh>
#include <horizon/macosx/FUSE.hh>

namespace horizon
{
  namespace macosx
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    ParallelFUker::ParallelFUker()
    {
    }

    ///
    /// destructor.
    ///
    ParallelFUker::~ParallelFUker()
    {
    }

//
// ---------- fuker -----------------------------------------------------------
//

    ///
    /// this method sets up the fuker.
    ///
    elle::Status        ParallelFUker::Setup()
    {
      enter();

      // XXX to implement.

      leave();
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this callback is triggered whenever data is available on the
    /// FUSE socket.
    ///
    elle::Status        ParallelFUker::Event(const elle::Natural16&)
    {
      enter();

      // XXX to implement.

      leave();
    }

  }
}
