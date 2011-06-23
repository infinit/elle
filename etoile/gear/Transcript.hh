//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/gear/Transcript.hh
//
// created       julien quintard   [fri jun 17 13:10:45 2011]
// updated       julien quintard   [wed jun 22 13:50:03 2011]
//

#ifndef ETOILE_GEAR_TRANSCRIPT_HH
#define ETOILE_GEAR_TRANSCRIPT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

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
    class Transcript:
      public elle::Entity
    {
    public:
      //
      // methods
      //
      elle::Status	Push(const nucleus::Address&,
			     const nucleus::Block&);
      elle::Status	Wipe(const nucleus::Address&);

      //
      // attributes
      //
      // XXX Bucket		bucket;
    };

  }
}

#endif
