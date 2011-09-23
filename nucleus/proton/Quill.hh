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
#include <nucleus/proton/Nodule.hh>

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
      // classes
      //
      class Entry:
	public elle::Object
      {
      public:
	//
	// constructors & destructors
	//
	Entry(V*);
	Entry(const Address&);
	~Entry();

	//
	// interfaces
	//

	// dumpable
	elle::Status	Dump(const elle::Natural32 = 0) const;

	//
	// attributes
	//
	Address		address;
	V*		value;
      };

      //
      // types
      //
      typedef std::map<const typename V::K, Entry*>	Container;
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
      elle::Status		Add(const typename V::K&,
				    Entry*);

      elle::Status		Lookup(const typename V::K&,
				       V*&) const;
      elle::Status		Locate(const typename V::K&,
				       V*&) const;

      //
      // interfaces
      //

      // nodule
      elle::Status		Lookup(const typename V::K&,
				       Quill<V>*&);

      // dumpable
      elle::Status		Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      elle::Callback<
	elle::Status,
	elle::Parameters<
	  const Address&,
	  Nodule<V>*&
	  >
	>			load;
      elle::Callback<
	elle::Status,
	elle::Parameters<
	  const Address&,
	  const Nodule<V>*
	  >
	>			unload;

      Container			container;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <nucleus/proton/Quill.hxx>

#endif
