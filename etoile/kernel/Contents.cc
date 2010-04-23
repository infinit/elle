//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/Contents.cc
//
// created       julien quintard   [tue feb 17 12:39:45 2009]
// updated       julien quintard   [thu apr 22 17:25:37 2010]
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

    ///
    /// data-specific constructor.
    ///
    template <>
    Contents<Data>::Contents():
      ContentHashBlock::ContentHashBlock(hole::ComponentData),

      cipher(NULL),
      content(NULL)
    {
    }

    ///
    /// catalog-specific constructor.
    ///
    template <>
    Contents<Catalog>::Contents():
      ContentHashBlock::ContentHashBlock(hole::ComponentCatalog),

      cipher(NULL),
      content(NULL)
    {
    }

    ///
    /// reference-specific constructor.
    ///
    template <>
    Contents<Reference>::Contents():
      ContentHashBlock::ContentHashBlock(hole::ComponentReference),

      cipher(NULL),
      content(NULL)
    {
    }

  }
}

