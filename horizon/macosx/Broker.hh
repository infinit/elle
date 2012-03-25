//
// ---------- header ----------------------------------------------------------
//
// project       horizon
//
// license       infinit
//
// author        julien quintard   [sat mar 24 16:31:02 2012]
//

#ifndef HORIZON_MACOSX_BROKER_HH
#define HORIZON_MACOSX_BROKER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <horizon/macosx/Event.hh>

namespace horizon
{
  namespace macosx
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Broker:
      public ::QObject
    {
      Q_OBJECT;

    public:
      //
      // callbacks
      //

      // general purpose
      static elle::Status       Statfs(Event*);
      static elle::Status       Getattr(Event*);
      static elle::Status       Fgetattr(Event*);
      static elle::Status       Utimens(Event*);

      // directory
      static elle::Status       Opendir(Event*);
      static elle::Status       Readdir(Event*);
      static elle::Status       Releasedir(Event*);
      static elle::Status       Mkdir(Event*);
      static elle::Status       Rmdir(Event*);

      // access
      static elle::Status       Access(Event*);
      static elle::Status       Chmod(Event*);
      static elle::Status       Chown(Event*);

#if defined(HAVE_SETXATTR)
      // attribute
      static elle::Status       Setxattr(Event*);
      static elle::Status       Getxattr(Event*);
      static elle::Status       Listxattr(Event*);
      static elle::Status       Removexattr(Event*);
#endif

      // link
      static elle::Status       Symlink(Event*);
      static elle::Status       Readlink(Event*);

      // file
      static elle::Status       Create(Event*);
      static elle::Status       Open(Event*);
      static elle::Status       Write(Event*);
      static elle::Status       Read(Event*);
      static elle::Status       Truncate(Event*);
      static elle::Status       Ftruncate(Event*);
      static elle::Status       Release(Event*);

      // objects
      static elle::Status       Rename(Event*);
      static elle::Status       Unlink(Event*);

      //
      // slots
      //
      bool              event(QEvent*);
    };

  }
}

#endif
