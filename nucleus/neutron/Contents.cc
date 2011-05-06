//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/neutron/Contents.cc
//
// created       julien quintard   [tue feb 17 12:39:45 2009]
// updated       julien quintard   [thu may  5 13:36:07 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/neutron/Contents.hh>

namespace nucleus
{
  namespace neutron
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// the component identifier.
    ///
    template <typename T>
    const elle::String		Contents<T>::Name = "Contents";

    ///
    /// the component identifier specialised for the data, catalog
    /// and reference types.
    ///
    template <>
    const elle::String		Contents<Data>::Name =
      "Contents[Data]";
    template <>
    const elle::String		Contents<Catalog>::Name =
      "Contents[Catalog]";
    template <>
    const elle::String		Contents<Reference>::Name =
      "Contents[Reference]";

    ///
    /// data-specific constructor.
    ///
    template <>
    Contents<Data>::Contents():
      cipher(NULL),
      content(NULL)
    {
    }

    ///
    /// catalog-specific constructor.
    ///
    template <>
    Contents<Catalog>::Contents():
      cipher(NULL),
      content(NULL)
    {
    }

    ///
    /// reference-specific constructor.
    ///
    template <>
    Contents<Reference>::Contents():
      cipher(NULL),
      content(NULL)
    {
    }

  }
}

