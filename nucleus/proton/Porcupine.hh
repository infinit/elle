//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [mon sep 12 05:23:24 2011]
//

#ifndef NUCLEUS_PROTON_PORCUPINE_HH
#define NUCLEUS_PROTON_PORCUPINE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Nodule.hh>
#include <nucleus/proton/Quill.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class makes it easy to build a tree-based data structure of
    /// blocks which does not require all the blocks to remain in main
    /// memory.
    ///
    template <typename K,
	      typename V>
    class Porcupine:
      public elle::Object
    {
    public:
      //
      // constructors & destructors
      //
      Porcupine() {} // XXX
      Porcupine(const elle::Callback<
		  elle::Parameters<
		    const Address&,
		    V&
		    >
		  >&,
		const elle::Callback<
		  elle::Parameters<
		    const Address&,
		    const V&
		    >
		  >&);
      ~Porcupine();

      //
      // methods
      //
      elle::Status		Create();

      elle::Status		Add(const K&,
				    const V&);
      elle::Status		Modify(const K&,
				       const V&);
      elle::Status		Remove(const K&);

      elle::Status		Lookup(const K&,
				       Quill<K, V>*&);

      // XXX Lookup, Locate
      // XXX first, last, next, previous and foreach(LEAF OR INTERNAL OR BOTH)

      elle::Status		Grow(); // XXX: new root

      //
      // attributes
      //
      /* XXX
      elle::Callback<
	elle::Parameters<
	  >
	>			xload;
      elle::Callback<
	elle::Parameters<
	  >
	>			xunload;
      */
      Nodule*			root;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <nucleus/proton/Porcupine.hxx>

#endif
