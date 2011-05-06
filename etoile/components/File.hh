//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/components/File.hh
//
// created       julien quintard   [fri aug 14 18:57:08 2009]
// updated       julien quintard   [thu may  5 16:17:24 2011]
//

#ifndef ETOILE_COMPONENTS_ETOILE_HH
#define ETOILE_COMPONENTS_ETOILE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/components/Object.hh>

#include <etoile/context/File.hh>

namespace etoile
{
  namespace components
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides functionalities for managing directories.
    ///
    class File:
      public Object
    {
    public:
      //
      // static methods
      //
      static elle::Status	Create(context::File*);
      static elle::Status	Load(context::File*,
				     const nucleus::Address&);
      static elle::Status	Write(context::File*,
				      const nucleus::Offset&,
				      const elle::Region&);
      static elle::Status	Read(context::File*,
				     const nucleus::Offset&,
				     const nucleus::Size&,
				     elle::Region&);
      static elle::Status	Adjust(context::File*,
				       const nucleus::Size&);
      static elle::Status	Discard(context::File*);
      static elle::Status	Store(context::File*);
      static elle::Status	Destroy(context::File*);
    };

  }
}

#endif
