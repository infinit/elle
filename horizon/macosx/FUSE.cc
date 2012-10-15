#include <horizon/macosx/FUSE.hh>
#include <horizon/macosx/FUker.hh>

#include <elle/Exception.hh>
#include <elle/idiom/Open.hh>

namespace horizon
{
  namespace macosx
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
      if (FUker::Initialize() == elle::Status::Error)
        escape("unable to initialize the FUker");

      return elle::Status::Ok;
    }

    ///
    /// this method cleans the system module.
    ///
    elle::Status        FUSE::Clean()
    {
      // clean the FUker.
      if (FUker::Clean() == elle::Status::Error)
        escape("unable to clean the FUker");

      return elle::Status::Ok;
    }

  }
}
