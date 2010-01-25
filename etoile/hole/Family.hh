//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/hole/Family.hh
//
// created       julien quintard   [fri dec 18 13:40:40 2009]
// updated       julien quintard   [fri dec 18 13:41:15 2009]
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
	FamilyUnknown,
	FamilyContentHashBlock,
	FamilyPublicKeyBlock,

	Families = FamilyPublicKeyBlock + 1
      };

  }
}

#endif
