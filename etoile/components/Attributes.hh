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
// updated       julien quintard   [mon apr 19 14:41:09 2010]
//

#ifndef ETOILE_COMPONENTS_ATTRIBUTES_HH
#define ETOILE_COMPONENTS_ATTRIBUTES_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/kernel/Permissions.hh>
#include <etoile/kernel/Subject.hh>
#include <etoile/kernel/Token.hh>

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
      static Status	Set(context::Object*,
			    const String&,
			    const String&);
      static Status	Get(context::Object*,
			    const String&,
			    kernel::Trait*&);
      static Status	Fetch(context::Object*,
			      kernel::Range<kernel::Trait>&);
      static Status	Omit(context::Object*,
			     const String&);
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/kernel/State.hh>

#include <etoile/depot/Depot.hh>

#include <etoile/components/Rights.hh>

#endif
