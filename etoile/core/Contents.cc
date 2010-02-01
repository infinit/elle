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
// updated       julien quintard   [mon feb  1 00:45:31 2010]
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
    const String		Contents<T>::Identifier = "Contents";

    ///
    /// the component identifier specialised for the data, catalog
    /// and reference types.
    ///
    template <>
    const String		Contents<Data>::Identifier =
      "Contents[Data]";
    template <>
    const String		Contents<Catalog>::Identifier =
      "Contents[Catalog]";
    template <>
    const String		Contents<Reference>::Identifier =
      "Contents[Reference]";

  }
}

