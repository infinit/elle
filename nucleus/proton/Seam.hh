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
    /// XXX internal nodule
    ///
    template <typename V>
    class Seam:
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
	Entry(const typename V::K&,
	      const Address&);
	~Entry();

	//
	// attributes
	//
	typename V::K	key;
	Address		address;
	Nodule<V>*	nodule;
      };

      //
      // types
      //
      typedef std::vector<Entry*>			Container;
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
      // interfaces
      //

      // nodule
      elle::Status		Lookup(const typename V::K&,
				       Quill<V>*&);

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
      elle::Natural32		size;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <nucleus/proton/Seam.hxx>

#endif
