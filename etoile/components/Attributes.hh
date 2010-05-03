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
// updated       julien quintard   [mon may  3 17:56:24 2010]
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
      static elle::Status	Set(context::Object*,
				    const elle::String&,
				    const elle::String&);
      static elle::Status	Get(context::Object*,
				    const elle::String&,
				    kernel::Trait*&);
      static elle::Status	Fetch(context::Object*,
				      kernel::Range<kernel::Trait>&);
      static elle::Status	Omit(context::Object*,
				     const elle::String&);
    };

  }
}

#endif
