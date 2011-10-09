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
    /// XXX
    ///
    template <typename... T>
    class Quill;

    ///
    /// XXX leaf node
    ///
    template <typename V>
    class Quill<V>:
      public Nodule<V>
    {
    public:
      //
      // static attributes
      //
      static elle::Natural32	Footprint;

      //
      // static methods
      //
      static elle::Status	Initialize();
      static elle::Status	Clean();

      //
      // types
      //
      typedef Inlet<V, V>				I;

      //
      // types
      //
      typedef std::map<const typename V::K, I*>			Container;

      struct							Iterator
      {
	typedef typename Container::iterator			Forward;
	typedef typename Container::reverse_iterator		Backward;
      };

      struct							Scoutor
      {
	typedef typename Container::const_iterator		Forward;
	typedef typename Container::const_reverse_iterator	Backward;
      };

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
      ~Quill();

      //
      // methods
      //
      elle::Status		Create();

      elle::Status		Insert(const typename V::K&,
				       V*);
      elle::Status		Insert(I*);

      elle::Status		Delete(typename Iterator::Forward&);
      elle::Status		Delete(V*);
      elle::Status		Delete(const typename V::K&);

      elle::Status		Lookup(const typename V::K&,
				       typename Iterator::Forward&);
      elle::Status		Lookup(const typename V::K&,
				       I*&);
      elle::Status		Lookup(const typename V::K&,
				       V*&);

      elle::Status		Locate(const typename V::K&,
				       typename Iterator::Forward&);
      elle::Status		Locate(const typename V::K&,
				       I*&);
      elle::Status		Locate(const typename V::K&,
				       V*&);

      elle::Status		Split(Quill<V>*&);
      elle::Status		Balance();

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
