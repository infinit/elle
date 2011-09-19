//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [fri sep 16 20:56:15 2011]
//

#ifndef NUCLEUS_PROTON_QUILL_HXX
#define NUCLEUS_PROTON_QUILL_HXX

namespace nucleus
{
  namespace proton
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// XXX
    ///
    template <typename V>
    Quill<V>::Quill():
      Nodule<V>(Nodule<V>::TypeQuill),

      size(0)
    {
    }

    ///
    /// XXX
    ///
    template <typename V>
    Quill<V>::Quill(const elle::Callback<
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
      Nodule<V>(Nodule<V>::TypeQuill),

      size(0),

      load(load),
      unload(unload)
    {
    }

//
// ---------- nodule ----------------------------------------------------------
//

    ///
    /// XXX
    ///
    template <typename V>
    elle::Status	Quill<V>::Lookup(const typename V::K&,
					 Quill<V>*&		quill)
    {
      enter();

      // return this quill.
      quill = this;

      leave();
    }

  }
}

#endif
