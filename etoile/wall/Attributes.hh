//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/wall/Attributes.hh
//
// created       julien quintard   [wed mar 31 19:23:49 2010]
// updated       julien quintard   [thu may  5 16:30:12 2011]
//

#ifndef ETOILE_WALL_ATTRIBUTES_HH
#define ETOILE_WALL_ATTRIBUTES_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/context/Identifier.hh>

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
    class Attributes
    {
    public:
      //
      // static methods
      //
      static elle::Status	Set(const context::Identifier&,
				    const elle::String&,
				    const elle::String&);
      static elle::Status	Get(const context::Identifier&,
				    const elle::String&);
      static elle::Status	Fetch(const context::Identifier&);
      static elle::Status	Omit(const context::Identifier&,
				     const elle::String&);
    };

  }
}

#endif
