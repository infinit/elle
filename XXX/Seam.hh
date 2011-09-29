//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [mon sep 12 06:15:32 2011]
//

#ifndef NUCLEUS_PROTON_SEAM_HH
#define NUCLEUS_PROTON_SEAM_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <nucleus/proton/Address.hh>
#include <XXX/Nodule.hh>
#include <XXX/Quill.hh>
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
    /// XXX internal nodule
    ///
    template <typename V>
    class Seam:
      public Nodule<V>
    {
    public:
      //
      // types
      //
      typedef Inlet< V, Nodule<V> >			I;

      //
      // types
      //
      typedef std::map<const typename V::K, I*>		Container;
      typedef typename Container::iterator		Iterator;
      typedef typename Container::const_iterator	Scoutor;

      //
      // constructors & destructors
      //
      Seam();
      Seam(const elle::Callback<
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
      ~Seam();

      //
      // methods
      //
      elle::Status		Create();

      elle::Status		Insert(const typename V::K&,
				       Nodule<V>*);
      elle::Status		Insert(I*);

      elle::Status		Delete(const typename V::K&);
      elle::Status		Delete(Iterator&);

      elle::Status		Lookup(const typename V::K&,
				       Iterator&);
      elle::Status		Lookup(const typename V::K&,
				       I*&);
      elle::Status		Lookup(const typename V::K&,
				       Nodule<V>*&);

      elle::Status		Locate(const typename V::K&,
				       Iterator&);
      elle::Status		Locate(const typename V::K&,
				       I*&);
      elle::Status		Locate(const typename V::K&,
				       Nodule<V>*&);

      elle::Status		Split(Seam<V>*&);

      elle::Status		Update(const typename V::K&);

      //
      // interfaces
      //

      // nodule
      elle::Status		Major(typename V::K&) const;
      elle::Status		Search(const typename V::K&,
				       Quill<V>*&);

      // dumpable
      elle::Status		Dump(const elle::Natural32 = 0) const;

      // XXX archivable

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

#include <XXX/Seam.hxx>

#endif
