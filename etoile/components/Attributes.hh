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
// updated       julien quintard   [thu apr 15 18:54:26 2010]
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
      static Status	Add(context::Object*,
			    const String&,
			    const String&);
      static Status	Lookup(context::Object*,
			       const String&,
			       kernel::Trait*&);
      static Status	Consult(context::Object*,
				const kernel::Index&,
				const kernel::Size&,
				kernel::Collection&);
      static Status	Update(context::Object*,
			       const String&,
			       const String&);
      static Status	Remove(context::Object*,
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
