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
// updated       julien quintard   [thu may  5 16:31:51 2011]
//

#ifndef ETOILE_WALL_FILE_HH
#define ETOILE_WALL_FILE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/path/Way.hh>

#include <etoile/context/Identifier.hh>

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
      static elle::Status	Create();
      static elle::Status	Load(const path::Way&);
      static elle::Status	Lock(const context::Identifier&);
      static elle::Status	Release(const context::Identifier&);
      static elle::Status	Write(const context::Identifier&,
				      const nucleus::Offset&,
				      const elle::Region&);
      static elle::Status	Read(const context::Identifier&,
				     const nucleus::Offset&,
				     const nucleus::Size&);
      static elle::Status	Adjust(const context::Identifier&,
				       const nucleus::Size&);
      static elle::Status	Discard(const context::Identifier&);
      static elle::Status	Store(const context::Identifier&);
      static elle::Status	Destroy(const context::Identifier&);
    };

  }
}

#endif
