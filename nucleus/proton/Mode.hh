#ifndef NUCLEUS_PROTON_MODE_HH
# define NUCLEUS_PROTON_MODE_HH

namespace nucleus
{
  namespace proton
  {
    /*-------------.
    | Enumerations |
    `-------------*/

    /// Define the mode in which content is generally represented being
    /// either empty, directly through a value, through a single
    /// block of data or via a complete tree of blocks.
    enum class Mode
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
                Mode const mode);
  }
}

#endif
