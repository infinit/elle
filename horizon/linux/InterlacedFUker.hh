//
// ---------- header ----------------------------------------------------------
//
// project       facade
//
// license       infinit
//
// author        julien quintard   [tue jul 26 15:33:15 2011]
//

#ifndef FACADE_LINUX_INTERLACEDFUKER_HH
#define FACADE_LINUX_INTERLACEDFUKER_HH

//
// ---------- macros ----------------------------------------------------------
//

#ifndef FUSE_USE_VERSION
# define FUSE_USE_VERSION               26
#endif

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <facade/linux/FUker.hh>

#include <elle/idiom/Close.hh>
# include <list>
# include <fuse/fuse.h>
# include <fuse/fuse_lowlevel.h>
#include <elle/idiom/Open.hh>

namespace facade
{
  namespace linux
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this FUker is said to handle events in an interlaced manner because
    /// every time a FUSE upcall is triggered, the FUker reads it and triggers
    /// it right away.
    ///
    /// assuming the previous event has not returned yet, for instance because
    /// a network communication is in process, two events could be being
    /// processed at the same time, leading to potential conflicts when
    /// returning i.e FUSE expects the return value of the first upcall but
    /// the second one terminates first.
    ///
    /// this FUker should therefore never be used unless the user knows
    /// exactly what he is doing.
    ///
    class InterlacedFUker:
      public FUker
    {
    public:
      //
      // constructors & destructors
      //
      InterlacedFUker();
      ~InterlacedFUker();

      //
      // interfaces
      //

      // fuker
      elle::Status      Setup();

      //
      // callbacks
      //
      elle::Status      Event(elle::Natural16);

      //
      // attributes
      //
      elle::Broker*             broker;

      struct ::fuse*            fuse;
      char*                     mountpoint;
      struct ::fuse_session*    session;
      struct ::fuse_chan*       channel;
      size_t                    size;
    };

  }
}

#endif
