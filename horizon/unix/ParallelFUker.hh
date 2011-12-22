//
// ---------- header ----------------------------------------------------------
//
// project       facade
//
// license       infinit
//
// author        julien quintard   [tue jul 26 15:33:15 2011]
//

#ifndef FACADE_UNIX_PARALLELFUKER_HH
#define FACADE_UNIX_PARALLELFUKER_HH

//
// ---------- macros ----------------------------------------------------------
//

#ifndef FUSE_USE_VERSION
# define FUSE_USE_VERSION		26
#endif

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <facade/unix/FUker.hh>

namespace facade
{
#undef unix
  namespace unix
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX to implement.
    ///
    class ParallelFUker:
      public FUker
    {
    public:
      //
      // constructors & destructors
      //
      ParallelFUker();
      ~ParallelFUker();

      //
      // interfaces
      //

      // fuker
      elle::Status	Setup();

      //
      // callbacks
      //
      elle::Status	Event(const elle::Natural16&);
    };

  }
}

#endif
