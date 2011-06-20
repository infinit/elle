//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/gear/Context.hh
//
// created       julien quintard   [thu jun 16 09:55:55 2011]
// updated       julien quintard   [mon jun 20 12:34:33 2011]
//

#ifndef ETOILE_GEAR_CONTEXT_HH
#define ETOILE_GEAR_CONTEXT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/gear/Nature.hh>
#include <etoile/gear/Transcribable.hh>

namespace etoile
{
  ///
  /// XXX
  ///
  namespace gear
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// an context contains the information related to a set of blocks.
    ///
    /// automata are then triggered on these contexts in order to perform
    /// some modifications.
    ///
    class Context:
      public elle::Object,
      virtual public Transcribable
    {
    public:
      //
      // constructors & destructors
      //
      Context(const Nature);

      //
      // interfaces
      //

      // XXX object

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      // archivable
      // XXX

      //
      // attributes
      //
      Nature		nature;
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

// XXX

#endif
