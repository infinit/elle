//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/kernel/Kernel.hh
//
// created       julien quintard   [mon feb 16 16:46:27 2009]
// updated       julien quintard   [mon apr 19 14:25:06 2010]
//

#ifndef ETOILE_KERNEL_KERNEL_HH
#define ETOILE_KERNEL_KERNEL_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace etoile
{
  ///
  /// this namespace contains the base functionalities for handling
  /// Etoile's kernel components such as objects for instance.
  ///
  namespace kernel
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Kernel
    {
    public:
      //
      // static methods
      //
      static Status	Initialize();
      static Status	Clean();
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/kernel/ContentHashBlock.hh>
#include <etoile/kernel/PublicKeyBlock.hh>
#include <etoile/kernel/Object.hh>
#include <etoile/kernel/Contents.hh>
#include <etoile/kernel/Catalog.hh>
#include <etoile/kernel/Reference.hh>
#include <etoile/kernel/Data.hh>
#include <etoile/kernel/Access.hh>
#include <etoile/kernel/Attributes.hh>
#include <etoile/kernel/Entry.hh>
#include <etoile/kernel/Record.hh>
#include <etoile/kernel/Trait.hh>
#include <etoile/kernel/Genre.hh>
#include <etoile/kernel/Version.hh>
#include <etoile/kernel/Subject.hh>
#include <etoile/kernel/Component.hh>
#include <etoile/kernel/State.hh>
#include <etoile/kernel/Offset.hh>
#include <etoile/kernel/Permissions.hh>
#include <etoile/kernel/Token.hh>
#include <etoile/kernel/Proof.hh>
#include <etoile/kernel/Voucher.hh>

#endif
