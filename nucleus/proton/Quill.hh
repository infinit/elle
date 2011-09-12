//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/proton/Quill.hh
//
// created       julien quintard   [mon sep 12 05:53:08 2011]
// updated       julien quintard   [mon sep 12 07:34:32 2011]
//

#ifndef NUCLEUS_PROTON_QUILL_HH
#define NUCLEUS_PROTON_QUILL_HH

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
    /// XXX leaf node
    ///
    template <typename K,
	      typename V>
    class Quill:
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
      Chassis<K, V>*		content;
      elle::Natural32		size;
    };

  }
}

#endif
