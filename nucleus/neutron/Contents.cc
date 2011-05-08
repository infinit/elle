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
// updated       julien quintard   [sun may  8 08:58:42 2011]
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

  }
}

