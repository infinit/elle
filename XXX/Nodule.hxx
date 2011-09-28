//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [sun sep 18 14:35:06 2011]
//

#ifndef NUCLEUS_PROTON_NODULE_HXX
#define NUCLEUS_PROTON_NODULE_HXX

namespace nucleus
{
  namespace proton
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    template <typename V>
    Nodule<V>::Nodule(const Type				type,
		      const elle::Callback<
			elle::Status,
			elle::Parameters<
			  const Address&,
			  Nodule<V>*&
			  >
			>&					load,
		      const elle::Callback<
			elle::Status,
			elle::Parameters<
			  const Address&,
			  const Nodule<V>*
			  >
			>&					unload):
      type(type),

      _load(load),
      _unload(unload),
      _parent(NULL),
      _left(NULL),
      _right(NULL),
      _footprint(*this)
    {
    }

  }
}

#endif
