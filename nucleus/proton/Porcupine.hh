//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/proton/Porcupine.hh
//
// created       julien quintard   [mon sep 12 05:23:24 2011]
// updated       julien quintard   [mon sep 12 07:35:25 2011]
//

#ifndef NUCLEUS_PROTON_PORCUPINE_HH
#define NUCLEUS_PROTON_PORCUPINE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Block.hh>
#include <nucleus/proton/Chassis.hh>
#include <nucleus/proton/Nodule.hh>

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
    template <typename A = Address,
	      typename K = Address,
	      typename V = Block*>
    class Porcupine:
      public elle::Object
    {
    public:
      //
      // types
      //
      typedef Chassis<A, elle::Void*>	Cell; // XXX

      //
      // constructors & destructors
      //
      Porcupine(const elle::Callback< elle::Parameters<const K&, V&>&,
		const elle::Callback< elle::Parameters<const K&, const V&>&,
		const elle::Callback< elle::Parameters<const K&, V&>&);

      //
      // methods
      //
      elle::Status		Create(const elle::Natural32);

      //
      // attributes
      //
      elle::Natural32		height;

      Nodule*			nodule;
    };

  }
}

#endif
