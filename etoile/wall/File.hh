//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/wall/File.hh
//
// created       julien quintard   [fri aug 14 15:36:23 2009]
// updated       julien quintard   [thu apr 22 11:06:05 2010]
//

#ifndef ETOILE_WALL_FILE_HH
#define ETOILE_WALL_FILE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/path/Way.hh>

#include <etoile/context/Identifier.hh>

#include <etoile/kernel/Offset.hh>
#include <etoile/kernel/Size.hh>

#include <etoile/components/File.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides functionalities for managing file objects.
    ///
    class File
    {
    public:
      //
      // static methods
      //
      static Status	Create();
      static Status	Load(const path::Way&);
      static Status	Lock(const context::Identifier&);
      static Status	Release(const context::Identifier&);
      static Status	Write(const context::Identifier&,
			      const kernel::Offset&,
			      const Region&);
      static Status	Read(const context::Identifier&,
			     const kernel::Offset&,
			     const kernel::Size&);
      static Status	Adjust(const context::Identifier&,
			       const kernel::Size&);
      static Status	Discard(const context::Identifier&);
      static Status	Store(const context::Identifier&);
      static Status	Destroy(const context::Identifier&);
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/context/Directory.hh>
#include <etoile/context/Format.hh>

#include <etoile/user/User.hh>

#include <etoile/path/Path.hh>

#endif
