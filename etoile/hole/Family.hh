//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/hole/Family.hh
//
// created       julien quintard   [fri dec 18 13:40:40 2009]
// updated       julien quintard   [wed mar 31 21:33:30 2010]
//

#ifndef ETOILE_HOLE_FAMILY_HH
#define ETOILE_HOLE_FAMILY_HH

namespace etoile
{
  namespace hole
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
