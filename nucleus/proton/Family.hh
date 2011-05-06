//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/proton/Family.hh
//
// created       julien quintard   [fri dec 18 13:40:40 2009]
// updated       julien quintard   [wed may  4 15:12:12 2011]
//

#ifndef NUCLEUS_PROTON_FAMILY_HH
#define NUCLEUS_PROTON_FAMILY_HH

namespace nucleus
{
  namespace proton
  {

//
// ---------- enumerations ----------------------------------------------------
//

    ///
    /// this enumeration defines the block's physical family.
    ///
    enum Family
      {
	FamilyUnknown = 0,

	FamilyContentHashBlock,
	FamilyPublicKeyBlock,

	Families = FamilyPublicKeyBlock + 1
      };

  }
}

#endif
