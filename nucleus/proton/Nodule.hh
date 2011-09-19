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

      //
      // attributes
      //
      Type		type;

      Seam<V>*		parent;
      Nodule*		left;
      Nodule*		right;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <nucleus/proton/Nodule.hxx>

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/proton/Seam.hh>
#include <nucleus/proton/Quill.hh>

#endif
