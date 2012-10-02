#ifndef NUCLEUS_PROTON_FAMILY_HH
# define NUCLEUS_PROTON_FAMILY_HH

# include <iosfwd>

namespace nucleus
{
  namespace proton
  {

    /*-------------.
    | Enumerations |
    `-------------*/

    /// Defines the block's physical family.
    enum class Family
      {
        content_hash_block,
        public_key_block,
        owner_key_block,
        imprint_block
      };

    /*----------.
    | Operators |
    `----------*/

    std::ostream&
    operator <<(std::ostream& stream,
                Family const family);

  }
}

#endif
