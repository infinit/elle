//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit
//
// file          /home/mycure/infinit/pig/FUSE.hh
//
// created       julien quintard   [fri jul 31 22:11:24 2009]
// updated       julien quintard   [wed jul  6 12:48:08 2011]
//

#ifndef PIG_FUSE_HH
#define PIG_FUSE_HH

//
// ---------- macros ----------------------------------------------------------
//

#ifndef FUSE_USE_VERSION
# define FUSE_USE_VERSION		26
#endif

//
// ---------- includes --------------------------------------------------------
//

#include <Infinit.hh>
#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>
#include <lune/Lune.hh>

#include <elle/idiom/Close.hh>
# include <fuse/fuse.h>
# include <fuse/fuse_lowlevel.h>
#include <elle/idiom/Open.hh>

namespace pig
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class contains everything related to FUSE.
  ///
  class FUSE
  {
  public:
    //
    // static methods
    //
    static elle::Status		Initialize();
    static elle::Status		Setup(const elle::String&);
    static elle::Status		Clean();

    //
    // static callbacks
    //
    static elle::Status		Event(const elle::Natural16&);

    //
    // static attributes
    //
    static elle::Broker*		Broker;

    struct				System
    {
      static struct ::fuse*		Core;
      static char*			Mountpoint;
      static struct ::fuse_session*	Session;
      static struct ::fuse_chan*	Channel;
      static size_t			Size;
      static char*			Buffer;
      static struct ::fuse_operations	Operations;
    };
  };

}

//
// ---------- structures ------------------------------------------------------
//

///
/// this structure is theoretically kept internally within the FUSE source
/// code.
///
/// unfortunately, since PIG is using FUSE's low-level functions, this
/// structure is needed to access the session (->se) field.
///
/// therefore this definition gives us just enough information in order
/// to access this field without having to define the rest of it.
///
struct fuse
{
  struct fuse_session *se;

  /* [...] */
};

#endif
