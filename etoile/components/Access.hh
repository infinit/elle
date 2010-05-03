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
// updated       julien quintard   [mon may  3 17:39:48 2010]
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
      static elle::Status	Open(context::Object*);
      static elle::Status	Grant(context::Object*,
				      const kernel::Subject&,
				      const kernel::Permissions&);
      static elle::Status	Lookup(context::Object*,
				       const kernel::Subject&,
				       kernel::Record*&);
      static elle::Status	Consult(context::Object*,
					const kernel::Index&,
					const kernel::Size&,
					kernel::Range<kernel::Record>&);
      static elle::Status	Update(context::Object*,
				       const kernel::Subject&,
				       const kernel::Permissions&);
      static elle::Status	Block(context::Object*,
				      const kernel::Subject&);
      static elle::Status	Revoke(context::Object*,
				       const kernel::Subject&);
      static elle::Status	Upgrade(context::Object*,
					const elle::SecretKey&);
      static elle::Status	Destroy(context::Object*);
      static elle::Status	Close(context::Object*);
    };

  }
}

#endif
