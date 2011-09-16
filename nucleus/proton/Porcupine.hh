//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [mon sep 12 05:23:24 2011]
//

#ifndef NUCLEUS_PROTON_PORCUPINE_HH
#define NUCLEUS_PROTON_PORCUPINE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Block.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class makes it easy to build a tree-based data structure of
    /// blocks which does not require all the blocks to remain in main
    /// memory.
    ///
    class Porcupine:
      public elle::Object
    {
    public:
      //
      // types
      //
      struct inentry{};
      struct lfentry{};

      bpt_make_types(porcupine, BPT_NDI_T, BPT_UNI_T, BPT_NODES_T,
		     BPT_HEIGHT_T, int, int, int*,
		     inentry, lfentry);
      bpt_make_protos(porcupine);

      //
      // constructors & destructors
      //
      Porcupine();

      //
      // methods
      //
      elle::Status		Create(const elle::Natural32);

      //
      // attributes
      //
      t_bpt(porcupine)*		bpt;
    };

  }
}

#endif
