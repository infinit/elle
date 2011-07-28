//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit
//
// file          /home/mycure/infinit/pig/ParallelFUker.cc
//
// created       julien quintard   [tue jul 26 15:33:54 2011]
// updated       julien quintard   [tue jul 26 20:52:22 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <pig/ParallelFUker.hh>
#include <pig/FUSE.hh>

namespace pig
{

//
// ---------- constructors & destructors --------------------------------------
//

  ///
  /// default constructor.
  ///
  ParallelFUker::ParallelFUker()
  {
    // XXX
  }

  ///
  /// destructor.
  ///
  ParallelFUker::~ParallelFUker()
  {
    // XXX
  }

//
// ---------- fuker -----------------------------------------------------------
//

  ///
  /// XXX
  ///
  elle::Status		ParallelFUker::Setup()
  {
    enter();

    // XXX

    leave();
  }

//
// ---------- callbacks -------------------------------------------------------
//

  ///
  /// this callback is triggered whenever data is available on the
  /// FUSE socket.
  ///
  elle::Status		ParallelFUker::Event(const elle::Natural16&)
  {
    enter();

    // XXX

    leave();
  }

}
