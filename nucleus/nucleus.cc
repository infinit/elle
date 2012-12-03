#include <nucleus/nucleus.hh>
#include <nucleus/Exception.hh>
#include <nucleus/proton/Contents.hh>
#include <nucleus/neutron/Object.hh>
#include <nucleus/neutron/Data.hh>
#include <nucleus/neutron/Catalog.hh>
#include <nucleus/neutron/Reference.hh>
#include <nucleus/neutron/Access.hh>
#include <nucleus/neutron/Group.hh>
#include <nucleus/neutron/Ensemble.hh>

#include <elle/assert.hh>
#include <elle/log.hh>

ELLE_LOG_COMPONENT("infinit.nucleus");

namespace nucleus
{
  namespace factory
  {
    /*----------.
    | Functions |
    `----------*/

    static
    elle::utility::Factory<neutron::Component> const*
    _setup_block()
    {
      elle::utility::Factory<neutron::Component>* factory =
        new elle::utility::Factory<neutron::Component>;

      ELLE_TRACE("setting up the nucleus block factory");

      factory->record<neutron::Object>(neutron::ComponentObject);
      factory->record<proton::Contents>(
        neutron::ComponentContents); // XXX[should be in neutron?]
      factory->record<neutron::Access>(neutron::ComponentAccess);
      factory->record<neutron::Group>(neutron::ComponentGroup);
      factory->record<neutron::Ensemble>(neutron::ComponentEnsemble);

      return (factory);
    }

    elle::utility::Factory<neutron::Component> const&
    block()
    {
      static elle::utility::Factory<neutron::Component> const* factory =
        _setup_block();

      ELLE_ASSERT(factory != nullptr);

      return (*factory);
    }

    static
    elle::utility::Factory<proton::Breed> const*
    _setup_node()
    {
      elle::utility::Factory<proton::Breed>* factory =
        new elle::utility::Factory<proton::Breed>;

      ELLE_TRACE("setting up the nucleus node factory");

      factory->record<proton::Seam<neutron::Catalog>>(
        Contents::Type::catalog_seam);
      factory->record<proton::Quill<neutron::Catalog>>(
        Contents::Type::catalog_quill);
      factory->record<neutron::Catalog>(
        Contents::Type::catalog_value);

      return (factory);
    }

    elle::utility::Factory<proton::Breed> const&
    node()
    {
      static elle::utility::Factory<proton::Breed> const* factory =
        _setup_node();

      ELLE_ASSERT(factory != nullptr);

      return (*factory);
    }

}
