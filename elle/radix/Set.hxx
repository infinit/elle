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
// updated       julien quintard   [fri jul 15 15:55:29 2011]
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
    /// union
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
