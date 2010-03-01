//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/misc/Maid.cc
//
// created       julien quintard   [sun feb 28 09:20:18 2010]
// updated       julien quintard   [mon mar  1 14:51:38 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/misc/Maid.hh>

namespace elle
{
  namespace misc
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this constant represents the maximum size of a guard instance.
    ///
    /// indeed, since the memory is allocated on the stack via alloca() and
    /// since this is done in a macro-function, hence without any type-specific
    /// information, the worst-case scenario is assmumed i.e that the guard
    /// will keep track of nine pointers.
    ///
    const Natural32		Maid::Shell::Size =
      sizeof(Maid::Guard< Maid::Routine<Void*, Void (*)(Void*) >* >,
	     Maid::Guard< Maid::Routine<Void*, Void (*)(Void*) >* >,
	     Maid::Guard< Maid::Routine<Void*, Void (*)(Void*) >* >,
	     Maid::Guard< Maid::Routine<Void*, Void (*)(Void*) >* >,
	     Maid::Guard< Maid::Routine<Void*, Void (*)(Void*) >* >,
	     Maid::Guard< Maid::Routine<Void*, Void (*)(Void*) >* >,
	     Maid::Guard< Maid::Routine<Void*, Void (*)(Void*) >* >,
	     Maid::Guard< Maid::Routine<Void*, Void (*)(Void*) >* >,
	     Maid::Guard< Maid::Routine<Void*, Void (*)(Void*) >* >);

    ///
    /// XXX
    ///
    const Natural32		Maid::Object::Size =
      sizeof(Maid::Routine<Void*, Void (*)(Void*)>);
  }
}
