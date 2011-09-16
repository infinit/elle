//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [fri sep 16 13:33:45 2011]
//

#ifndef NUCLEUS_PROTON_PORCUPINE_HXX
#define NUCLEUS_PROTON_PORCUPINE_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/proton/Quill.hh>

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
    template <typename K,
	      typename V>
    Porcupine<K, V>::Porcupine(const elle::Callback<
				 elle::Parameters<
				   const Address&,
				   V&
				   >
				 >&				load,
			       const elle::Callback<
				 elle::Parameters<
				   const Address&,
				   const V&
				   >
				 >&				unload):
      load(load),
      unload(unload),
      nodule(NULL)
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
    elle::Status	Porcupine<K, V>::Create()
    {
      Quill<K, V>*	quill;

      enter(instance(quill));

      // at first, allocate an initial root nodule which happens to be
      // a leaf since alone.
      quill = new Quill<K, V>;

      // create the quill.
      if (quill->Create() == elle::StatusError)
	escape("unable to create the quill");

      // set the root nodule.
      this->nodule = quill;

      // waive the tracking.
      waive(quill);

      leave();
    }

  }
}

#endif
