//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/proton/Base.hh
//
// created       julien quintard   [sat may 21 13:21:51 2011]
// updated       julien quintard   [sat may 21 15:28:43 2011]
//

#ifndef NUCLEUS_PROTON_BASE_HH
#define NUCLEUS_PROTON_BASE_HH

namespace nucleus
{
  namespace proton
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Base
    {
    public:
      //
      // attributes
      //
      Version		version;
      elle::Digest*	digest;
    };

  }
}

#endif
