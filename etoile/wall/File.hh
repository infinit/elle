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
// updated       julien quintard   [tue jun 14 14:52:21 2011]
//

#ifndef ETOILE_WALL_FILE_HH
#define ETOILE_WALL_FILE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/path/Way.hh>

#include <etoile/gear/Identifier.hh>

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
      static elle::Status	Create(gear::Identifier&);
      static elle::Status	Load(const path::Way&,
				     gear::Identifier&);
      static elle::Status	Lock(const gear::Identifier&);
      static elle::Status	Release(const gear::Identifier&);
      static elle::Status	Write(const gear::Identifier&,
				      const nucleus::Offset&,
				      const elle::Region&);
      static elle::Status	Read(const gear::Identifier&,
				     const nucleus::Offset&,
				     const nucleus::Size&,
				     elle::Region&);
      static elle::Status	Adjust(const gear::Identifier&,
				       const nucleus::Size&);
      static elle::Status	Discard(const gear::Identifier&);
      static elle::Status	Store(const gear::Identifier&);
      static elle::Status	Destroy(const gear::Identifier&);
    };

  }
}

#endif
