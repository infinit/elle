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
      // classes
      //
      class Entry:
	public elle::Object
      {
      public:
	//
	// constructors & destructors
	//
	Entry(const Nodule<V>&);
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
	Nodule<V>*	nodule;
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

#include <nucleus/proton/Seam.hxx>

#endif
