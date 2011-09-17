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

#include <nucleus/proton/Nodule.hh>
#include <nucleus/proton/Chassis.hh>

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
    template <typename K,
	      typename V>
    class Quill:
      public Nodule
    {
    public:
      //
      // types
      //
      typedef std::vector< Chassis<K, V> >		Container;
      typedef typename Container::iterator		Iterator;
      typedef typename Container::const_iterator	Scoutor;

      //
      // constructors & destructors
      //
      Quill();

      //
      // methods
      //
      elle::Status		Lookup(const K&,
				       Quill<K, V>*&);

      //
      // attributes
      //
      Container			container;
      elle::Natural32		size;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <nucleus/proton/Quill.hxx>

#endif
