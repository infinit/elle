//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [fri aug 14 18:57:08 2009]
//

#ifndef ETOILE_AUTOMATON_FILE_HH
#define ETOILE_AUTOMATON_FILE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

namespace etoile
{
  namespace gear
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// this class needs to be forwarded declared in order to prevent
    /// conflits.
    ///
    class File;

  }

  namespace automaton
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides functionalities for managing directories.
    ///
    class File
    {
    public:
      //
      // static methods
      //
      static elle::Status	Create(gear::File&);
      static elle::Status	Load(gear::File&);

      static elle::Status	Write(gear::File&,
				      const nucleus::Offset&,
				      const elle::Region&);
      static elle::Status	Read(gear::File&,
				     const nucleus::Offset&,
				     const nucleus::Size&,
				     elle::Region&);
      static elle::Status	Adjust(gear::File&,
				       const nucleus::Size&);

      static elle::Status	Discard(gear::File&);
      static elle::Status	Destroy(gear::File&);
      static elle::Status	Store(gear::File&);
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/gear/File.hh>

#endif
