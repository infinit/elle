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

      elle::Status		Add(const typename V::K&,
				    I*);

      //
      // interfaces
      //

      // nodule
      elle::Status		Lookup(const typename V::K&,
				       Quill<V>*&);
      elle::Status		Root(Nodule<V>*&) const;

      // dumpable
      elle::Status		Dump(const elle::Natural32 = 0) const;

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

#include <XXX/Seam.hxx>

#endif
