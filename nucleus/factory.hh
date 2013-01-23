#ifndef NUCLEUS_FACTORY_HH
# define NUCLEUS_FACTORY_HH

# include <elle/types.hh>
# include <elle/utility/Factory.hh>

# include <nucleus/proton/Nature.hh>
# include <nucleus/neutron/Component.hh>

namespace nucleus
{
  namespace factory
  {
    /*----------.
    | Functions |
    `----------*/

    /// Return the factory capable of building nucleus block instances.
    elle::utility::Factory<neutron::Component> const&
    block();

    /// Return the factory capable of building tree node instances.
    elle::utility::Factory<proton::Nature> const&
    node();
  }
}

#endif
