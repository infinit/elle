#ifndef NUCLEUS_PROTON_NATURE_HH
# define NUCLEUS_PROTON_NATURE_HH

namespace nucleus
{
  namespace proton
  {
    /*-------------.
    | Enumerations |
    `-------------*/

    /// Define the nature in which content is generally represented being
    /// either empty, directly through a value, through a single
    /// block of data or via a complete tree of blocks.
    enum class Nature
      {
        empty,
        value,
        block,
        tree
      };

    /*----------.
    | Operators |
    `----------*/

    std::ostream&
    operator <<(std::ostream& stream,
                Nature const nature);
  }
}

#endif
