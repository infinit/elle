#ifndef HOLE_OPENNESS_HH
# define HOLE_OPENNESS_HH

# include <iosfwd>

namespace hole
{

  /// The openness defines how users can join a network.
  ///
  /// A closed network is controlled by its sole owner who can
  /// decide who joins and who does not. A community network
  /// is more flexible as any member can decide to invite other
  /// users. Finally, an open network is open to anyone.
  enum class Openness
  {
    open = 1,
    community,
    closed
  };

  std::ostream&
  operator <<(std::ostream& stream, Openness openness);

}

#endif
