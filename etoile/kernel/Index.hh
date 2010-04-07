//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/Index.hh
//
// created       julien quintard   [thu apr  1 22:36:01 2010]
// updated       julien quintard   [fri apr  2 13:32:50 2010]
//

#ifndef ETOILE_KERNEL_INDEX_HH
#define ETOILE_KERNEL_INDEX_HH

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/kernel/Size.hh>

namespace etoile
{
  namespace kernel
  {

//
// ---------- types -----------------------------------------------------------
//

    ///
    /// this type is an alias of the size and has been introduced
    /// for clarity when representing indexes.
    ///
    typedef Size		Index;

//
// ---------- constants -------------------------------------------------------
//

    ///
    /// this constants defines the first and last indexes.
    ///
    const Index			IndexFirst = Type<Index>::Minimum;
    const Index			IndexLast = Type<Index>::Maximum;

  }
}

#endif
