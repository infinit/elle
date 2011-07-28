//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit
//
// file          /home/mycure/infinit/pig/InterlacedFUker.hh
//
// created       julien quintard   [tue jul 26 15:33:15 2011]
// updated       julien quintard   [wed jul 27 08:19:36 2011]
//

#ifndef PIG_INTERLACEDFUKER_HH
#define PIG_INTERLACEDFUKER_HH

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

#include <pig/FUker.hh>

#include <elle/idiom/Close.hh>
# include <list>
# include <fuse/fuse.h>
# include <fuse/fuse_lowlevel.h>
#include <elle/idiom/Open.hh>

namespace pig
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// XXX
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
    elle::Status	Setup();

    //
    // callbacks
    //
    elle::Status	Event(const elle::Natural16&);

    //
    // attributes
    //
    elle::Broker*		broker;

    struct ::fuse*		fuse;
    char*			mountpoint;
    struct ::fuse_session*	session;
    struct ::fuse_chan*		channel;
    size_t			size;
  };

}

#endif
