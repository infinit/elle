//
// ---------- header ----------------------------------------------------------
//
// project       horizon
//
// license       infinit
//
// author        julien quintard   [fri jul 31 22:11:24 2009]
//

#ifndef HORIZON_MACOSX_FUSE_HH
#define HORIZON_MACOSX_FUSE_HH

//
// ---------- macros ----------------------------------------------------------
//

#ifndef FUSE_USE_VERSION
# define FUSE_USE_VERSION               26
#endif

//
// ---------- includes --------------------------------------------------------
//

#include <Infinit.hh>
#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>
#include <lune/Lune.hh>

#include <horizon/macosx/FUker.hh>

#include <elle/idiom/Close.hh>
# define _GNU_SOURCE
# include <fuse/fuse.h>
#include <elle/idiom/Open.hh>

namespace horizon
{
  namespace macosx
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class contains everything related to FUSE.
    ///
    /// note that this implementation is based on FUSE4x.
    ///
    class FUSE
    {
    public:
      //
      // static methods
      //
      static elle::Status       Initialize(const struct ::fuse_operations&);
      static elle::Status       Setup(const elle::String&);
      static elle::Status       Clean();

      //
      // static attributes
      //
      static elle::String               Mountpoint;
      static FUker*                     Fuker;

      static struct ::fuse_operations   Operations;
    };

  }
}

//
// ---------- structures ------------------------------------------------------
//

///
/// this structure is theoretically kept internally within the FUSE source
/// code.
///
/// unfortunately, since horizon is using FUSE's low-level functions, this
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

/// XXX
#define __u64 uint64_t
#define __s64 int64_t
#define __u32 uint32_t
#define __s32 int32_t
#define __u16 uint16_t

struct fuse_in_header {
  __u32   len;
  __u32   opcode;
  __u64   unique;
  __u64   nodeid;
  __u32   uid;
  __u32   gid;
  __u32   pid;
  __u32   padding;
};

#endif
