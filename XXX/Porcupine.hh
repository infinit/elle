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
    template <typename... T>
    class Seam;

    template <typename... T>
    class Quill;

//
// ---------- classes ---------------------------------------------------------
//

    // XXX
    template <typename... T>
    class Porcupine;

    ///
    /// this class makes it easy to build a tree-based data structure of
    /// blocks which does not require all the blocks to remain in main
    /// memory.
    ///
    template <typename V>
    class Porcupine<V>:
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
      elle::Status		Locate(const typename V::K&,
				       V*&);
      elle::Status		Remove(const typename V::K&);

      template <typename N,
		typename W>
      elle::Status		Insert(N*,
				       const typename V::K&,
				       W*);
      template <typename N>
      elle::Status		Delete(N*,
				       const typename V::K&);

      elle::Status		Grow();
      elle::Status		Shrink();

      elle::Status		Search(const typename V::K&,
				       Quill<V>*&);

      elle::Status		Load(const Address&,
				     Nodule<V>*&);
      elle::Status		Unload(const Address&,
				       const Nodule<V>*);

      //
      // interfaces
      //

      // dumpable
      elle::Status		Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      elle::Natural32		height;
      Address			root;

      elle::Callback<
	elle::Status,
	elle::Parameters<
	  const Address&,
	  Block&
	  >
	>			_load;
      elle::Callback<
	elle::Status,
	elle::Parameters<
	  const Address&,
	  const Block&
	  >
	>			_unload;
      Nodule<V>*		_root;
    };

    ///
    /// XXX
    ///
    template <>
    class Porcupine<>
    {
    public:
      //
      // static methods
      //
      static elle::Status	Initialize();
      static elle::Status	Clean();
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
