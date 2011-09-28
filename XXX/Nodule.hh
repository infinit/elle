//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [mon sep 12 05:46:11 2011]
//

#ifndef NUCLEUS_PROTON_NODULE_HH
#define NUCLEUS_PROTON_NODULE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <nucleus/proton/ContentHashBlock.hh>

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
    template <typename V>
    class Seam;

    template <typename V>
    class Quill;

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    template <typename V>
    class Nodule:
      public ContentHashBlock
    {
    public:
      //
      // enumerations
      //
      enum Type
	{
	  TypeSeam,
	  TypeQuill
	};

      //
      // constructors & destructors
      //
      Nodule(const Type,
	     const elle::Callback<
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
      // virtual methods
      //
      virtual elle::Status	Major(typename V::K&) const = 0;
      virtual elle::Status	Lookup(const typename V::K&,
				       Quill<V>*&) = 0;

      //
      // attributes
      //
      Type			type;

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
      Seam<V>*			_parent;
      Nodule*			_left;
      Nodule*			_right;
      elle::Footprint		_footprint;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <XXX/Nodule.hxx>

//
// ---------- includes --------------------------------------------------------
//

#include <XXX/Seam.hh>
#include <XXX/Quill.hh>

#endif
