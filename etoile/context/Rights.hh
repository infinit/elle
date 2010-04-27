//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/context/Rights.hh
//
// created       julien quintard   [tue feb  2 16:59:43 2010]
// updated       julien quintard   [mon apr 26 18:35:25 2010]
//

#ifndef ETOILE_CONTEXT_RIGHTS_HH
#define ETOILE_CONTEXT_RIGHTS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/kernel/Role.hh>
#include <etoile/kernel/Record.hh>

namespace etoile
{
  namespace context
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents the role, permissions and access key to the
    /// object and its contents.
    ///
    class Rights:
      public Meta,
      public Dumpable
    {
    public:
      //
      // constructors & destructors
      //
      Rights();

      //
      // interfaces
      //

      // dumpable
      Status			Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      kernel::Role		role;
      kernel::Record		record;
      SecretKey			key;
    };

  }
}

#endif
