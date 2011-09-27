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
#include <nucleus/proton/Block.hh>
#include <XXX/Nodule.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// XXX
    ///
    template <typename V>
    class Seam;

    template <typename V>
    class Quill;

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class makes it easy to build a tree-based data structure of
    /// blocks which does not require all the blocks to remain in main
    /// memory.
    ///
    template <typename V>
    class Porcupine:
      public elle::Object
    {
    public:
      //
      // constructors & destructors
      //
      Porcupine(const elle::Callback<
		  elle::Status,
		  elle::Parameters<
		    const Address&,
		    Block&
		    >
		  >&,
		const elle::Callback<
		  elle::Status,
		  elle::Parameters<
		    const Address&,
		    const Block&
		    >
		  >&);
      ~Porcupine();

      //
      // methods
      //
      elle::Status		Create();

      elle::Status		Add(const typename V::K&,
				    V*);
      elle::Status		Modify(const typename V::K&,
				       V*);
      elle::Status		Remove(const typename V::K&);

      elle::Status		Lookup(const typename V::K&,
				       Quill<V>*&) const;
      elle::Status		Locate(const typename V::K&,
				       Quill<V>*&,
				       V*&) const;

      elle::Status		Load(const Address&,
				     Nodule<V>*&);
      elle::Status		Unload(const Address&,
				       const Nodule<V>*);

      // XXX Lookup, Locate
      // XXX first, last, next, previous and foreach(LEAF OR INTERNAL OR BOTH)

      //
      // interfaces
      //

      // dumpable
      elle::Status		Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      elle::Callback<
	elle::Status,
	elle::Parameters<
	  const Address&,
	  Block&
	  >
	>			load;
      elle::Callback<
	elle::Status,
	elle::Parameters<
	  const Address&,
	  const Block&
	  >
	>			unload;

      Nodule<V>*		root;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <XXX/Porcupine.hxx>

//
// ---------- includes --------------------------------------------------------
//

#include <XXX/Seam.hh>
#include <XXX/Quill.hh>

#endif
