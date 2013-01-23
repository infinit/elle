#ifndef NUCLEUS_PROTON_STRATEGY_HH
# define NUCLEUS_PROTON_STRATEGY_HH

# include <iosfwd>

namespace nucleus
{
  namespace proton
  {
    /*-------------.
    | Enumerations |
    `-------------*/

    /// Define the strategy in which content is represented being
    /// either none (when there is no actual content), directly through
    /// an embedded value, through a single block of encrypted data or
    /// via a complete tree of blocks.
    enum class Strategy
      {
        none,
        value,
        block,
        tree
      };

    /*----------.
    | Operators |
    `----------*/

    std::ostream&
    operator <<(std::ostream& stream,
                Strategy const strategy);
  }
}

#endif
