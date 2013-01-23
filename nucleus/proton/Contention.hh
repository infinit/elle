#ifndef NUCLEUS_PROTON_CONTENTION_HH
# define NUCLEUS_PROTON_CONTENTION_HH

# include <elle/types.hh>

namespace nucleus
{
  namespace proton
  {
    /// Define the size of the content which must be kept in a block
    /// when split.
    ///
    /// For example, given an extent of 1024 and a contention of 0.4,
    /// and assuming the block has reached a footprint higher than
    /// its extent so that it needs splitting, a new block would be
    /// created and content would be transferred to it so as to maintain
    /// approximately 410 bytes (1024 * 0.4) in the original block.
    typedef elle::Real Contention;
  }
}

#endif
