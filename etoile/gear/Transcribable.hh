//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/gear/Transcribable.hh
//
// created       julien quintard   [fri jun 17 13:07:46 2011]
// updated       julien quintard   [mon jun 20 12:34:44 2011]
//

#ifndef ETOILE_GEAR_TRANSCRIBABLE_HH
#define ETOILE_GEAR_TRANSCRIBABLE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/gear/Transcript.hh>

namespace etoile
{
  namespace gear
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Transcribable
    {
    public:
      //
      // methods
      //
      virtual elle::Status	Transcribe(Transcript&) const = 0;
    };

  }
}

#endif
