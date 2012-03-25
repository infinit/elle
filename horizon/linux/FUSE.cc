//
// ---------- header ----------------------------------------------------------
//
// project       horizon
//
// license       infinit
//
// author        julien quintard   [fri jul 31 22:10:21 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <horizon/linux/FUSE.hh>
#include <horizon/linux/Crux.hh>
#include <horizon/linux/InterlacedFUker.hh>
#include <horizon/linux/ParallelFUker.hh>
#include <horizon/linux/SequentialFUker.hh>

#include <Infinit.hh>

namespace horizon
{
  namespace linux
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this structure contains the function pointers to the FUSE
    /// implementation routines located in the crux.
    ///
    struct ::fuse_operations            FUSE::Operations;

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes the FUSE component.
    ///
    elle::Status        FUSE::Initialize(
                          const struct ::fuse_operations&       operations)
    {
      // set the fuse operations.
      FUSE::Operations = operations;

      // initialize the FUker.
      if (FUker::Initialize() == elle::StatusError)
        escape("unable to initialize the FUker");

      return elle::StatusOk;
    }

    ///
    /// this method cleans the system module.
    ///
    elle::Status        FUSE::Clean()
    {
      // clean the FUker.
      if (FUker::Clean() == elle::StatusError)
        escape("unable to clean the FUker");

      return elle::StatusOk;
    }

  }
}
