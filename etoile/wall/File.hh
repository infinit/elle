//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/wall/File.hh
//
// created       julien quintard   [fri aug 14 15:36:23 2009]
// updated       julien quintard   [sat apr  3 23:33:32 2010]
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
    /// XXX
    ///
    class File
    {
    public:
      //
      // static methods
      //
      static Status	Load(const path::Way&);
      static Status	Lock(const context::Identifier&);
      static Status	Release(const context::Identifier&);
      static Status	Create(const context::Identifier&,
			       const path::Slice&,
			       const context::Identifier&);
      static Status	Read(const context::Identifier&,
			     const kernel::Offset&,
			     const kernel::Size&);
      static Status	Write(const context::Identifier&,
			      const kernel::Offset&,
			      const Region&,
			      const kernel::Size&);
      static Status	Store(const context::Identifier&);
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/context/File.hh>

#include <etoile/user/User.hh>

#endif
