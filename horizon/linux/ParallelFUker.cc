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

#include <horizon/linux/ParallelFUker.hh>
#include <horizon/linux/FUSE.hh>

namespace horizon
{
  namespace linux
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
      // XXX to implement.

      return elle::StatusOk;
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
      // XXX to implement.

      return elle::StatusOk;
    }

  }
}
