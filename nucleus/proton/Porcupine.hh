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

#include <nucleus/proton/Nodule.hh>

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

      //
      // methods
      //
      elle::Status		Create();

      elle::Status		Add(const K&,
				    const V&);
      elle::Status		Modify(const K&,
				       const V&);
      elle::Status		Remove(const K&);

      // XXX Lookup, Locate
      // XXX first, last, next, previous and foreach(LEAF OR INTERNAL OR BOTH)

      elle::Status		Grow(); // XXX: new root

      //
      // attributes
      //
      elle::Callback<
	elle::Parameters<
	  const Address&,
	  V&
	  >
	>			load;
      elle::Callback<
	elle::Parameters<
	  const Address&,
	  const V&
	  >
	>			unload;

      Nodule*			nodule;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <nucleus/proton/Porcupine.hxx>

#endif
