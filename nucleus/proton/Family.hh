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
// updated       julien quintard   [thu may 12 12:58:21 2011]
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
	FamilyOwnerKeyBlock,
	FamilyImprintBlock,

	Families = FamilyImprintBlock + 1
      };

  }
}

#endif
