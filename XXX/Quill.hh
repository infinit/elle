//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [mon sep 12 05:53:08 2011]
//

#ifndef NUCLEUS_PROTON_QUILL_HH
#define NUCLEUS_PROTON_QUILL_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Block.hh>
#include <XXX/Nodule.hh>
#include <XXX/Inlet.hh>

#include <elle/idiom/Close.hh>
# include <map>
#include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX leaf node
    ///
    template <typename V>
    class Quill:
      public Nodule<V>
    {
    public:
      //
      // types
      //
      typedef Inlet<V, V>				I;

      //
      // types
      //
      typedef std::map<const typename V::K, I*>		Container;
      typedef typename Container::iterator		Iterator;
      typedef typename Container::const_iterator	Scoutor;

      //
      // constructors & destructors
      //
      Quill();
      Quill(const elle::Callback<
	      elle::Status,
	      elle::Parameters<
		const Address&,
		Nodule<V>*&
		>
	      >&,
	    const elle::Callback<
	      elle::Status,
	      elle::Parameters<
		const Address&,
		const Nodule<V>*
		>
	      >&);

      //
      // methods
      //
      elle::Status		Create();

      elle::Status		Add(const typename V::K&,
				    I*);

      elle::Status		Lookup(const typename V::K&,
				       V*&) const;
      elle::Status		Locate(const typename V::K&,
				       V*&) const;

      elle::Status		Split();
      elle::Status		Root(Nodule<V>*&) const;

      //
      // interfaces
      //

      // nodule
      elle::Status		Lookup(const typename V::K&,
				       Quill<V>*&);

      // dumpable
      elle::Status		Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status		Serialize(elle::Archive&) const;
      elle::Status		Extract(elle::Archive&);

      //
      // attributes
      //
      Container			container;

      elle::Callback<
	elle::Status,
	elle::Parameters<
	  const Address&,
	  Nodule<V>*&
	  >
	>			_load;
      elle::Callback<
	elle::Status,
	elle::Parameters<
	  const Address&,
	  const Nodule<V>*
	  >
	>			_unload;
      elle::Footprint		_footprint;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <XXX/Quill.hxx>

#endif
