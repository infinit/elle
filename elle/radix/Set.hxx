//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/radix/Set.hxx
//
// created       julien quintard   [fri jul 15 14:51:42 2011]
// updated       julien quintard   [sat sep  3 20:57:31 2011]
//

#ifndef ELLE_RADIX_SET_HXX
#define ELLE_RADIX_SET_HXX

namespace elle
{
  namespace radix
  {

//
// ---------- specializations -------------------------------------------------
//

    ///
    /// this template structure returns a Parameters representing the
    /// union of the inputs Parameters types.
    ///
    template <typename... T1,
	      typename... T2>
    struct Set::Union< Parameters<T1...>,
		       Parameters<T2...> >
    {
      typedef Parameters<T1..., T2...>		Type;
    };

  }
}

#endif
