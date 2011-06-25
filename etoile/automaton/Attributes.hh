//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/automaton/Attributes.hh
//
// created       julien quintard   [mon feb  1 19:22:40 2010]
// updated       julien quintard   [sat jun 25 16:24:27 2011]
//

#ifndef ETOILE_AUTOMATON_ATTRIBUTES_HH
#define ETOILE_AUTOMATON_ATTRIBUTES_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/gear/Object.hh>

namespace etoile
{
  namespace automaton
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides functionalities for manipulating an
    /// object's attributes.
    ///
    class Attributes
    {
    public:
      //
      // static methods
      //
      static elle::Status	Set(gear::Object&,
				    const elle::String&,
				    const elle::String&);
      static elle::Status	Get(gear::Object&,
				    const elle::String&,
				    nucleus::Trait&);
      static elle::Status	Fetch(gear::Object&,
				      nucleus::Range<nucleus::Trait>&);
      static elle::Status	Omit(gear::Object&,
				     const elle::String&);
    };

  }
}

#endif
