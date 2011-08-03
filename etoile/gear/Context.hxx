//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/gear/Context.hxx
//
// created       julien quintard   [fri jun 24 12:25:31 2011]
// updated       julien quintard   [fri jul 29 14:35:48 2011]
//

#ifndef ETOILE_GEAR_CONTEXT_HXX
#define ETOILE_GEAR_CONTEXT_HXX

namespace etoile
{
  namespace gear
  {

//
// ---------- types -----------------------------------------------------------
//

    ///
    /// this base template allows for specialized-template.
    ///
    template <const Nature N>
    struct ContextType
    {
    };

    ///
    /// this macro-function links the type to the enum value in a simple
    /// call.
    ///
#define ContextDeclare(_type_)						\
  template <>								\
  struct ContextType<Nature ## _type_>					\
  {									\
    typedef _type_				T;			\
  };

  }
}

#endif
