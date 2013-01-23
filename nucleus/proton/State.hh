#ifndef NUCLEUS_PROTON_STATE_HH
# define NUCLEUS_PROTON_STATE_HH

# include <iosfwd>

namespace nucleus
{
  namespace proton
  {
    /*-------------.
    | Enumerations |
    `-------------*/

    /// Specify the state of an in-memory block.
    ///
    /// The dirty state indicates that the block has been modified while the
    /// consistent state indicates that the block has changed but the
    /// required adjustements have since been performed for the block to be
    /// stored.
    enum class State
      {
        clean = 0,
        dirty,
        consistent
      };

    /*----------.
    | Operators |
    `----------*/

    std::ostream&
    operator <<(std::ostream& stream,
                State const state);
  }
}

#endif
