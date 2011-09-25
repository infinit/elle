//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author           [fri sep 23 10:51:23 2011]
//

#ifndef ELLE_PACKAGE_FOOTPRINT_HH
#define ELLE_PACKAGE_FOOTPRINT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/package/Archive.hh>

namespace elle
{
  namespace package
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class emulates an archive in order to catch the Store()
    /// calls for every basic type being serialized.
    ///
    /// note that should a footprint object be used, no serialization is
    /// performed. instead, the footprint mechanism computes the sum of
    /// the types' size.
    ///
    /// this way, one can know in advance how large is going to be the
    /// object once serialized without actually performing the serialization
    /// process.
    ///
    class Footprint:
      public Archive
    {
    public:
      //
      // constants
      //
      static const Natural32		Meta;

      //
      // static methods
      //
      static Status	Compute(const Archivable&,
				Natural32&);

      //
      // constructors & destructors
      //
      Footprint();

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // object-like
      template <typename T>
      Status		Recycle(const T* = NULL);

      virtual Status	Imprint(Natural32&) const;
      virtual Status	Clone(Footprint*&) const;

      Footprint&	operator=(const Footprint&);
      Boolean		operator==(const Footprint&) const;
      Boolean		operator!=(const Footprint&) const;

      // archive
      Status		Store(const Null&);
      Status		Store(const Boolean&);
      Status		Store(const Character&);
      Status		Store(const Real&);
      Status		Store(const Integer8&);
      Status		Store(const Integer16&);
      Status		Store(const Integer32&);
      Status		Store(const Integer64&);
      Status		Store(const Natural8&);
      Status		Store(const Natural16&);
      Status		Store(const Natural32&);
      Status		Store(const Natural64&);
      Status		Store(const Large&);
      Status		Store(const String&);
      Status		Store(const Region&);
      Status		Store(const Archive&);

      Status		Load(Null&);
      Status		Load(Boolean&);
      Status		Load(Character&);
      Status		Load(Real&);
      Status		Load(Integer8&);
      Status		Load(Integer16&);
      Status		Load(Integer32&);
      Status		Load(Integer64&);
      Status		Load(Natural8&);
      Status		Load(Natural16&);
      Status		Load(Natural32&);
      Status		Load(Natural64&);
      Status		Load(Large&);
      Status		Load(String&);
      Status		Load(Region&);
      Status		Load(Archive&);

      //
      // attributes
      //
      Natural32		size;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/package/Footprint.hxx>

#endif
