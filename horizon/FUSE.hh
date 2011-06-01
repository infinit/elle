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
// updated       julien quintard   [wed jun  1 10:03:24 2011]
//

#ifndef PIG_FUSE_HH
#define PIG_FUSE_HH

//
// ---------- macros ----------------------------------------------------------
//

#define FUSE_USE_VERSION		26

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
    static elle::Status		Initialize(const elle::String&,
					   const elle::String&,
					   const elle::String&);
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

//
// ---------- macro-functions -------------------------------------------------
//

#define error(_text_, _errno_, _identifiers_...)			\
  do									\
    {									\
      report(elle::standalone::Report::TypeError, _text_);		\
									\
      show();								\
									\
      FUSE::Release(_identifiers_);					\
									\
      purge();								\
									\
      return (-(_errno_));						\
    } while (false)

#define skip(_errno_, _identifiers_...)					\
  do									\
    {									\
      FUSE::Release(_identifiers_);					\
									\
      purge();								\
									\
      return (-(_errno_));						\
    } while (false)

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
