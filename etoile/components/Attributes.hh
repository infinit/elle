//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/components/Attributes.hh
//
// created       julien quintard   [mon feb  1 19:22:40 2010]
// updated       julien quintard   [thu may  5 16:16:17 2011]
//

#ifndef ETOILE_COMPONENTS_ATTRIBUTES_HH
#define ETOILE_COMPONENTS_ATTRIBUTES_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/context/Object.hh>

namespace etoile
{
  namespace components
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides functionalities for manipulating the attributes.
    ///
    class Attributes
    {
    public:
      //
      // static methods
      //
      static elle::Status	Set(context::Object*,
				    const elle::String&,
				    const elle::String&);
      static elle::Status	Get(context::Object*,
				    const elle::String&,
				    nucleus::Trait*&);
      static elle::Status	Fetch(context::Object*,
				      nucleus::Range<nucleus::Trait>&);
      static elle::Status	Omit(context::Object*,
				     const elle::String&);
    };

  }
}

#endif
