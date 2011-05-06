//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/neutron/Offset.hh
//
// created       julien quintard   [sun jan 31 22:34:41 2010]
// updated       julien quintard   [thu may  5 09:11:25 2011]
//

#ifndef NUCLEUS_NEUTRON_OFFSET_HH
#define NUCLEUS_NEUTRON_OFFSET_HH

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/neutron/Size.hh>

namespace nucleus
{
  namespace neutron
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
