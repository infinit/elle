//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [mon sep 12 06:30:17 2011]
//

#ifndef NUCLEUS_PROTON_CHASSIS_HH
#define NUCLEUS_PROTON_CHASSIS_HH

namespace nucleus
{
  namespace proton
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX nodule entry
    ///
    template <typename K,
	      typename V>
    class Chassis:
      public elle::Object
    {
    public:
      //
      // attributes
      //
      K			key;
      V			value;
    };

  }
}

#endif
