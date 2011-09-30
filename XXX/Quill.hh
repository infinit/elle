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

      elle::Status		Insert(const typename V::K&,
				       V*);
      elle::Status		Insert(I*);

      elle::Status		Delete(Iterator&);
      elle::Status		Delete(V*);
      elle::Status		Delete(const typename V::K&);

      elle::Status		Lookup(const typename V::K&,
				       Iterator&);
      elle::Status		Lookup(const typename V::K&,
				       I*&);
      elle::Status		Lookup(const typename V::K&,
				       V*&);

      elle::Status		Locate(const typename V::K&,
				       Iterator&);
      elle::Status		Locate(const typename V::K&,
				       I*&);
      elle::Status		Locate(const typename V::K&,
				       V*&);

      elle::Status		Split(Quill<V>*&);

      //
      // interfaces
      //

      // nodule
      elle::Status		Major(typename V::K&) const;
      elle::Status		Search(const typename V::K&,
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
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <XXX/Quill.hxx>

#endif
