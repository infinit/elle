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

#include <nucleus/proton/Seam.hh>
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
    /* XXX
      load(load),
      unload(unload),
    */
      root(NULL)
    {
    }

    ///
    /// destructor.
    ///
    template <typename K,
	      typename V>
    Porcupine<K, V>::~Porcupine()
    {
      // delete the root nodule, if present.
      if (this->root != NULL)
	delete this->root;
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

      // set the root nodule.
      this->root = quill;

      // waive the tracking.
      waive(quill);

      leave();
    }

    ///
    /// XXX
    ///
    template <typename K,
	      typename V>
    elle::Status	Porcupine<K, V>::Add(const K&		key,
					     const V&		value)
    {
      Quill<K, V>*	quill;

      enter();

      // look up for the quill responsible for the given key.
      if (this->Lookup(key, quill) == elle::StatusError)
	escape("unable to locate a quill for this key");

      // XXX

      leave();
    }

    ///
    /// XXX
    ///
    template <typename K,
	      typename V>
    elle::Status	Porcupine<K, V>::Lookup(const K&	key,
						Quill<K, V>*&	quill)
    {
      enter();

      // trigger the lookup method on the root nodule, depending on its type.
      switch (this->root->type)
	{
	case Nodule::TypeSeam:
	  {
	    Seam<K>*		nodule;

	    // cast the nodule.
	    nodule = static_cast< Seam<K>* >(this->root);

	    if (nodule->Lookup(key, quill) == elle::StatusError)
	      escape("unable to locate the quill for this key");

	    break;
	  }
	case Nodule::TypeQuill:
	  {
	    Quill<K, V>*	nodule;

	    // cast the nodule.
	    nodule = static_cast< Quill<K, V>* >(this->root);

	    if (nodule->Lookup(key, quill) == elle::StatusError)
	      escape("unable to locate the quill for this key");

	    break;
	  }
	}

      leave();
    }

  }
}

#endif
