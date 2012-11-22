#ifndef NUCLEUS_NUCLEUS_HH
# define NUCLEUS_NUCLEUS_HH

# include <nucleus/neutron/Component.hh>

# include <elle/types.hh>
# include <elle/utility/Factory.hh>

/// This namespace contains the whole logic related to the Infinit
/// file system, especially the blocks composing the file system.
///
/// Note that many classes define a static attribute named null which
/// is used by methods to indicate that the requested information
/// as not be found though such a behavior is not considered "exceptional".
namespace nucleus
{
  /*----------.
  | Functions |
  `----------*/

  /// Return the factory capable of building nucleus class instances.
  elle::utility::Factory<neutron::Component> const&
  factory();
}

#endif
