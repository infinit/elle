//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/wall/Attribute.hh
//
// created       julien quintard   [wed mar 31 19:23:49 2010]
// updated       julien quintard   [sat apr  3 23:29:27 2010]
//

#ifndef ETOILE_WALL_ATTRIBUTE_HH
#define ETOILE_WALL_ATTRIBUTE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/context/Identifier.hh>

#include <etoile/kernel/Index.hh>
#include <etoile/kernel/Size.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides functionalities for controlling the attributes
    /// attached to objects.
    ///
    class Attribute
    {
    public:
      //
      // static methods
      //
      static Status	Add(const context::Identifier&,
			    const String&,
			    const String&);
      static Status	Exist(const context::Identifier&,
			      const String&);
      static Status	Lookup(const context::Identifier&,
			       const String&);
      static Status	Consult(const context::Identifier&,
				const kernel::Index&,
				const kernel::Size&);
      static Status	Remove(const context::Identifier&,
			       const String&);
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/context/Object.hh>

#include <etoile/components/Attribute.hh>

#include <etoile/user/User.hh>

#endif
