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
// updated       julien quintard   [mon may  3 17:20:26 2010]
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
    const Offset		OffsetFirst = elle::Type<Offset>::Minimum;
    const Offset		OffsetLast = elle::Type<Offset>::Maximum;

  }
}

#endif
