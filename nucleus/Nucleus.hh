#ifndef NUCLEUS_NUCLEUS_HH
# define NUCLEUS_NUCLEUS_HH

# include <elle/types.hh>
# include <elle/utility/Factory.hh>

/// This namespace contains the whole logic related to the Infinit
/// file system, especially the blocks composing the file system.
namespace nucleus
{

  /// Returns the factory capable of building nucleus class instances.
  elle::utility::Factory const&
  factory();

}

#endif
