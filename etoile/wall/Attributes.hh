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
// updated       julien quintard   [mon jul  4 15:43:38 2011]
//

#ifndef ETOILE_WALL_ATTRIBUTES_HH
#define ETOILE_WALL_ATTRIBUTES_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/gear/Identifier.hh>

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
      static elle::Status	Set(const gear::Identifier&,
				    const elle::String&,
				    const elle::String&);
      static elle::Status	Get(const gear::Identifier&,
				    const elle::String&,
				    nucleus::Trait*&);
      static elle::Status	Fetch(const gear::Identifier&,
				      nucleus::Range<nucleus::Trait>&);
      static elle::Status	Omit(const gear::Identifier&,
				     const elle::String&);
    };

  }
}

#endif
