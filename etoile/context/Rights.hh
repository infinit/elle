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
// updated       julien quintard   [thu may  5 16:24:17 2011]
//

#ifndef ETOILE_CONTEXT_RIGHTS_HH
#define ETOILE_CONTEXT_RIGHTS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

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
      public elle::Entity
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
      elle::Status	Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      nucleus::Role		role;
      nucleus::Record		record;
      elle::SecretKey		key;
    };

  }
}

#endif
