#ifndef NUCLEUS_PROTON_FAMILY_HH
# define NUCLEUS_PROTON_FAMILY_HH

# include <iosfwd>

namespace nucleus
{
  namespace proton
  {

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

    std::ostream&
    operator <<(std::ostream& stream,
                Family family);

  }
}

#endif
