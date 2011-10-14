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
#include <nucleus/proton/State.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// this class needs forward declaration to prevent conflicts.
    ///
    template <typename... T>
    class Seam;

    ///
    /// this class needs forward declaration to prevent conflicts.
    ///
    template <typename... T>
    class Quill;

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class abstract the notion of tree node in a nodule.
    ///
    /// every nodule therefore encapsulates the type i.e seam or quill
    /// along with common information such as the addresses of the parent
    /// and neighbour nodules.
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
      // methods
      //
      template <typename T>
      elle::Status		Export(T*,
				       const elle::Natural32 = 0);
      template <typename T>
      elle::Status		Import(T*,
				       const elle::Natural32 = 0);

      template <typename T>
      elle::Status		Split(T*&);
      template <typename T>
      elle::Status		Merge(T*);

      //
      // virtual methods
      //
      virtual elle::Status	Mayor(typename V::K&) const = 0;
      virtual elle::Status	Maiden(typename V::K&) const = 0;
      virtual elle::Status	Search(const typename V::K&,
				       Quill<V>*&) = 0;
      virtual elle::Status	Check() const = 0;

      //
      // interfaces
      //

      // dumpable
      elle::Status		Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status		Serialize(elle::Archive&) const;
      elle::Status		Extract(elle::Archive&);

      //
      // attributes
      //
      Type			type;
      Address			parent;
      Address			left;
      Address			right;

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
