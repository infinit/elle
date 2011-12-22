//
// ---------- header ----------------------------------------------------------
//
// project       8diary
//
// license       infinit
//
// author        julien quintard   [sun jun 26 22:32:34 2011]
//

#ifndef DIARY_UNIX_MEMOIRS_HH
#define DIARY_UNIX_MEMOIRS_HH

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

#include <applications/8diary/unix/Upcall.hh>

#include <elle/idiom/Close.hh>
# include <fuse/fuse.h>
#include <elle/idiom/Open.hh>

namespace application
{
#undef unix
  namespace unix
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Memoirs:
      public elle::Object,
      public virtual elle::Fileable<elle::FormatRaw>
    {
    public:
      //
      // enumerations
      //
      enum Mode
	{
	  ModeUnknown,
	  ModeRecord,
	  ModeReplay
	};

      //
      // constructors & destructors
      //
      Memoirs();
      ~Memoirs();

      //
      // methods
      //
      elle::Status	Initialize(const elle::String&,
				   const elle::String&);
      elle::Status	Initialize(const elle::String&,
				   const elle::Natural32,
				   const elle::Natural32);
      elle::Status	Clean();

      elle::Status	Write(const Upcall&);
      elle::Status	Read(Upcall&);

      elle::Status	End() const;

      //
      // interfaces
      //

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status	Serialize(elle::Archive&) const;
      elle::Status	Extract(elle::Archive&);

      // fileable
      elle::Status	Load(const elle::Path&);
      elle::Status	Store(const elle::Path&) const;

      //
      // attributes
      //
      Mode			mode;

      ::fuse_operations		fuse;

      elle::Archive		archive;

      // XXX
      elle::String		mountpoint;

      struct
      {
	elle::Natural32		from;
	elle::Natural32		to;
      }				offsets;
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <applications/8diary/unix/Live.hh>
#include <applications/8diary/unix/Record.hh>
#include <applications/8diary/unix/Replay.hh>
#include <applications/8diary/unix/Upcall.hh>

#endif
