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
    template <typename K,
	      typename V>
    Quill<K, V>::Quill():
      Nodule(Nodule::TypeQuill),

      size(0)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    template <typename K,
	      typename V>
    elle::Status	Quill<K, V>::Lookup(const K&		key,
					    Quill<K, V>*&	quill)
    {
      enter();

      // return this quill.
      quill = this;

      leave();
    }

  }
}

#endif
