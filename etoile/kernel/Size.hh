//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/Size.hh
//
// created       julien quintard   [thu apr  1 22:36:01 2010]
// updated       julien quintard   [mon may  3 17:20:11 2010]
//

#ifndef ETOILE_KERNEL_SIZE_HH
#define ETOILE_KERNEL_SIZE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace etoile
{
  namespace kernel
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
