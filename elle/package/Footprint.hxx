//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [sat sep 24 23:35:29 2011]
//

#ifndef ELLE_PACKAGE_FOOTPRINT_HXX
#define ELLE_PACKAGE_FOOTPRINT_HXX

namespace elle
{
  namespace package
  {

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method computes the footprint of the given type.
    ///
    template <typename T>
    Status              Footprint::Compute(const T&             element,
                                           Natural32&           size)
    {
      Footprint         footprint;

      // create the footprint i.e archive in order to set it in
      // serialization mode.
      if (footprint.Create() == StatusError)
        escape("unable to create the footprint");

      // serialize the element with the footprint as argument so
      // as to compute its footprint.
      if (footprint.Serialize(element) == StatusError)
        escape("unable to serialize the element");

      // return the footprint's size.
      size = footprint.size;

      return elle::StatusOk;
    }

//
// ---------- object-like -----------------------------------------------------
//

    ///
    /// this method recycles a footprint.
    ///
    template <typename T>
    Status              Footprint::Recycle(const T*             object)
    {
      // release the resources.
      this->~Footprint();

      if (object == NULL)
        {
          // initialize the object with default values.
          new (this) T;
        }
      else
        {
          // initialize the object with defined values.
          new (this) T(*object);
        }

      // return elle::StatusOk in order to avoid including Report, Status and Maid.
      return (StatusOk);
    }

  }
}

#endif
