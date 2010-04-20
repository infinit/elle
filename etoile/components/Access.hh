//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/components/Access.hh
//
// created       julien quintard   [mon feb  1 19:22:40 2010]
// updated       julien quintard   [mon apr 19 14:33:27 2010]
//

#ifndef ETOILE_COMPONENTS_ACCESS_HH
#define ETOILE_COMPONENTS_ACCESS_HH

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
    /// this class provides functionalities for manipulating the access
    /// blocks.
    ///
    class Access
    {
    public:
      //
      // static methods
      //
      static Status	Open(context::Object*);
      static Status	Grant(context::Object*,
			      const kernel::Subject&,
			      const kernel::Permissions&);
      static Status	Lookup(context::Object*,
			       const kernel::Subject&,
			       kernel::Record*&);
      static Status	Consult(context::Object*,
				const kernel::Index&,
				const kernel::Size&,
				kernel::Range<kernel::Record>&);
      static Status	Update(context::Object*,
			       const kernel::Subject&,
			       const kernel::Permissions&);
      static Status	Block(context::Object*,
			      const kernel::Subject&);
      static Status	Revoke(context::Object*,
			       const kernel::Subject&);
      static Status	Upgrade(context::Object*,
				const SecretKey&);
      static Status	Destroy(context::Object*);
      static Status	Close(context::Object*);
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
