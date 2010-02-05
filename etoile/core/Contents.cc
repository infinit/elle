//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/Contents.cc
//
// created       julien quintard   [tue feb 17 12:39:45 2009]
// updated       julien quintard   [wed feb  3 22:12:27 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/core/Contents.hh>

namespace etoile
{
  namespace core
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

