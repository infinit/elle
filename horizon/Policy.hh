#ifndef HORIZON_POLICY_HH
# define HORIZON_POLICY_HH

# include <iostream>

namespace horizon
{

  /// The policy defines how file system objects (files, directories
  /// and links) are shared by default.
  ///
  /// An accessible policy renders every object stored within a network
  /// accessible in reading to all the other members of the network, no
  /// matter when they join. An editable policy makes every objects
  /// readable and writable by everybody. Finally, a confidential policy
  /// ensures that every file, directory etc. remains private by defaut.
  /// Then, the user can decide to share the files with other users and
  /// groups, in reading, writing or both.
  enum class Policy
  {
    accessible = 1,
    editable,
    confidential
  };

  std::ostream&
  operator <<(std::ostream& stream, Policy policy);

}

#endif
