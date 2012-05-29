//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [wed nov  2 09:11:57 2011]
//

#ifndef ETOILE_PORTAL_WRAPPER_HH
#define ETOILE_PORTAL_WRAPPER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/path/Slab.hh>

#include <etoile/gear/Identifier.hh>

namespace etoile
{
  namespace portal
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class contains wrappers for the purpose of translating pointers
    /// to internal objects into valid ready-to-be-serialized items.
    ///
    /// for more information on the actual implementation, please refer
    /// to the wall namespace.
    ///
    /// note that the original implementation has been kept in order
    /// to avoid relying on copies as it would impact the performance.
    ///
    class Wrapper
    {
    public:
      //
      // static methods
      //
      struct Access
      {
        static elle::Status     Lookup(const gear::Identifier&,
                                       const nucleus::Subject&,
                                       nucleus::Record&);
        static elle::Status     Consult(const gear::Identifier&,
                                        const nucleus::Index&,
                                        const nucleus::Size&,
                                        nucleus::Range<nucleus::Record>&);
      };

      struct Attributes
      {
        static elle::Status     Get(const gear::Identifier&,
                                    const elle::String&,
                                    nucleus::Trait&);
        static elle::Status     Fetch(const gear::Identifier&,
                                      nucleus::Range<nucleus::Trait>&);
      };

      struct Directory
      {
        static elle::Status     Lookup(const gear::Identifier&,
                                       const path::Slab&,
                                       nucleus::Entry&);
        static elle::Status     Consult(const gear::Identifier&,
                                        const nucleus::Index&,
                                        const nucleus::Size&,
                                        nucleus::Range<nucleus::Entry>&);
      };
    };

  }
}

#endif
