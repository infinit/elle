//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/Offset.hh
//
// created       julien quintard   [sun jan 31 22:34:41 2010]
// updated       julien quintard   [fri apr  2 00:01:35 2010]
//

#ifndef ETOILE_KERNEL_OFFSET_HH
#define ETOILE_KERNEL_OFFSET_HH

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
    /// this type defines the offset used in data for instance, and
    /// is an alias of the size.
    ///
    typedef Size		Offset;

//
// ---------- constants -------------------------------------------------------
//

    ///
    /// this constants defines the first and last offsets.
    ///
    const Offset		OffsetFirst = Type<Offset>::Minimum;
    const Offset		OffsetLast = Type<Offset>::Maximum;

  }
}

#endif
