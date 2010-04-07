//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/kernel/Contents.cc
//
// created       julien quintard   [tue feb 17 12:39:45 2009]
// updated       julien quintard   [sat mar 20 04:10:07 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/kernel/Contents.hh>

namespace etoile
{
  namespace kernel
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// the component identifier.
    ///
    template <typename T>
    const String		Contents<T>::Name = "Contents";

    ///
    /// the component identifier specialised for the data, catalog
    /// and reference types.
    ///
    template <>
    const String		Contents<Data>::Name =
      "Contents[Data]";
    template <>
    const String		Contents<Catalog>::Name =
      "Contents[Catalog]";
    template <>
    const String		Contents<Reference>::Name =
      "Contents[Reference]";

  }
}

