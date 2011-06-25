//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/automaton/File.hh
//
// created       julien quintard   [fri aug 14 18:57:08 2009]
// updated       julien quintard   [sat jun 25 14:31:41 2011]
//

#ifndef ETOILE_AUTOMATON_ETOILE_HH
#define ETOILE_AUTOMATON_ETOILE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/gear/File.hh>

namespace etoile
{
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
      static elle::Status	Load(gear::File&,
				     const nucleus::Location&);

      static elle::Status	Write(gear::File&,
				      const nucleus::Offset&,
				      const elle::Region&);
      static elle::Status	Read(gear::File&,
				     const nucleus::Offset&,
				     const nucleus::Size&,
				     elle::Region&);
      static elle::Status	Adjust(gear::File&,
				       const nucleus::Size&);

      static elle::Status	Destroy(gear::File&);
      static elle::Status	Store(gear::File&);
    };

  }
}

#endif
