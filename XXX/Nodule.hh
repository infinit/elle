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
      Nodule(const Type);

      //
      // virtual methods
      //
      virtual elle::Status	Lookup(const typename V::K&,
				       Quill<V>*&) = 0;
      virtual elle::Status	Root(Nodule<V>*&) const = 0;

      //
      // attributes
      //
      Type		type;

      Seam<V>*		_parent;
      Nodule*		_left;
      Nodule*		_right;
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
