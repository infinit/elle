//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/proton/Seam.hh
//
// created       julien quintard   [mon sep 12 06:15:32 2011]
// updated       julien quintard   [mon sep 12 07:34:35 2011]
//

#ifndef NUCLEUS_PROTON_SEAM_HH
#define NUCLEUS_PROTON_SEAM_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <nucleus/proton/Nodule.hh>
#include <nucleus/proton/Chassis.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX internal nodule
    ///
    template <typename K,
	      typename A>
    class Seam:
      public Nodule
    {
    public:
      //
      // methods
      //
      elle::Status		Create(const elle::Natural32);

      //
      // attributes
      //
      Chassis<K, A>*		content;
      elle::Natural32		size;
    };

  }
}

#endif
