//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/automaton/Access.hh
//
// created       julien quintard   [mon jun 20 14:58:16 2011]
// updated       julien quintard   [thu jun 23 14:45:09 2011]
//

#ifndef ETOILE_AUTOMATON_ACCESS_HH
#define ETOILE_AUTOMATON_ACCESS_HH

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
    /// this class provides a set of automata for manipulating the
    /// access control list embedded in the Access block referenced by
    /// the object.
    ///
    class Access
    {
    public:
      //
      // static methods
      //
      static elle::Status	Open(gear::Object&);

      static elle::Status	Grant(gear::Object&,
				      const nucleus::Subject&,
				      const nucleus::Permissions&);
      static elle::Status	Lookup(gear::Object&,
				       const nucleus::Subject&,
				       nucleus::Record&);
      static elle::Status	Consult(gear::Object&,
					const nucleus::Index&,
					const nucleus::Size&,
					nucleus::Range<nucleus::Record>&);
      static elle::Status	Update(gear::Object&,
				       const nucleus::Subject&,
				       const nucleus::Permissions&);
      static elle::Status	Revoke(gear::Object&,
				       const nucleus::Subject&);

      static elle::Status	Upgrade(gear::Object&,
					const elle::SecretKey&);
      static elle::Status	Downgrade(gear::Object&);

      static elle::Status	Destroy(gear::Object&);
      static elle::Status	Close(gear::Object&);
    };

  }
}

#endif
