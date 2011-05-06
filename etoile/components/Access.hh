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
// updated       julien quintard   [thu may  5 16:22:39 2011]
//

#ifndef ETOILE_COMPONENTS_ACCESS_HH
#define ETOILE_COMPONENTS_ACCESS_HH

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
				      const nucleus::Subject&,
				      const nucleus::Permissions&);
      static elle::Status	Lookup(context::Object*,
				       const nucleus::Subject&,
				       nucleus::Record*&);
      static elle::Status	Consult(context::Object*,
					const nucleus::Index&,
					const nucleus::Size&,
					nucleus::Range<nucleus::Record>&);
      static elle::Status	Update(context::Object*,
				       const nucleus::Subject&,
				       const nucleus::Permissions&);
      static elle::Status	Block(context::Object*,
				      const nucleus::Subject&);
      static elle::Status	Revoke(context::Object*,
				       const nucleus::Subject&);
      static elle::Status	Upgrade(context::Object*,
					const elle::SecretKey&);
      static elle::Status	Destroy(context::Object*);
      static elle::Status	Close(context::Object*);
    };

  }
}

#endif
