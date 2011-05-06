//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/neutron/Size.hh
//
// created       julien quintard   [thu apr  1 22:36:01 2010]
// updated       julien quintard   [thu may  5 13:26:17 2011]
//

#ifndef NUCLEUS_NEUTRON_SIZE_HH
#define NUCLEUS_NEUTRON_SIZE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace nucleus
{
  namespace neutron
  {

//
// ---------- types -----------------------------------------------------------
//

    ///
    /// this type represents the size of the contents being a catlaog,
    /// data or reference.
    ///
    typedef elle::Natural64		Size;

//
// ---------- constants -------------------------------------------------------
//

    ///
    /// this constants defines the minimum and maximum sizes.
    ///
    const Size			SizeMinimum = elle::Type<Size>::Minimum;
    const Size			SizeMaximum = elle::Type<Size>::Maximum;

  }
}

#endif
