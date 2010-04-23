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
// updated       julien quintard   [thu apr 22 11:09:31 2010]
//

#ifndef ETOILE_COMPONENTS_ETOILE_HH
#define ETOILE_COMPONENTS_ETOILE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/components/Object.hh>

#include <etoile/context/File.hh>

#include <etoile/hole/Address.hh>

#include <etoile/kernel/Catalog.hh>

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
      static Status	Create(context::File*);
      static Status	Load(context::File*,
			     const hole::Address&);
      static Status	Write(context::File*,
			      const kernel::Offset&,
			      const Region&);
      static Status	Read(context::File*,
			     const kernel::Offset&,
			     const kernel::Size&,
			     Region&);
      static Status	Adjust(context::File*,
			       const kernel::Size&);
      static Status	Discard(context::File*);
      static Status	Store(context::File*);
      static Status	Destroy(context::File*);
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/journal/Journal.hh>

#include <etoile/user/User.hh>

#include <etoile/components/Rights.hh>
#include <etoile/components/Contents.hh>

#endif
