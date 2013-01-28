#include <nucleus/factory.hh>
#include <nucleus/Exception.hh>
#include <nucleus/proton/Contents.hh>
#include <nucleus/proton/Seam.hh>
#include <nucleus/proton/Quill.hh>
#include <nucleus/neutron/Object.hh>
#include <nucleus/neutron/Data.hh>
#include <nucleus/neutron/Catalog.hh>
#include <nucleus/neutron/Reference.hh>
#include <nucleus/neutron/Access.hh>
#include <nucleus/neutron/Group.hh>
#include <nucleus/neutron/Ensemble.hh>
#include <nucleus/neutron/Attributes.hh>

#include <elle/assert.hh>
#include <elle/log.hh>

ELLE_LOG_COMPONENT("infinit.nucleus.factory");

namespace nucleus
{
  namespace factory
  {
    namespace setup
    {
      /*----------.
      | Functions |
      `----------*/

      static
      elle::utility::Factory<neutron::Component> const*
      _block()
      {
        elle::utility::Factory<neutron::Component>* factory =
          new elle::utility::Factory<neutron::Component>;

        ELLE_TRACE("setting up the nucleus block factory");

        factory->record<neutron::Object>(neutron::ComponentObject);
        // XXX[shouldn't be in neutron?]
        factory->record<proton::Contents>(neutron::ComponentContents);
        factory->record<neutron::Group>(neutron::ComponentGroup);

        return (factory);
      }

      static
      elle::utility::Factory<proton::Nature> const*
      _node()
      {
        elle::utility::Factory<proton::Nature>* factory =
          new elle::utility::Factory<proton::Nature>;

        ELLE_TRACE("setting up the nucleus node factory");

        factory->record<proton::Seam<neutron::Data>>(
          proton::Nature::data_seam);
        factory->record<proton::Quill<neutron::Data>>(
          proton::Nature::data_quill);
        factory->record<neutron::Data>(
          proton::Nature::data_value);

        factory->record<proton::Seam<neutron::Catalog>>(
          proton::Nature::catalog_seam);
        factory->record<proton::Quill<neutron::Catalog>>(
          proton::Nature::catalog_quill);
        factory->record<neutron::Catalog>(
          proton::Nature::catalog_value);

        // XXX[reference]

        factory->record<proton::Seam<neutron::Attributes>>(
          proton::Nature::attributes_seam);
        factory->record<proton::Quill<neutron::Attributes>>(
          proton::Nature::attributes_quill);
        factory->record<neutron::Attributes>(
          proton::Nature::attributes_value);

        factory->record<proton::Seam<neutron::Access>>(
          proton::Nature::access_seam);
        factory->record<proton::Quill<neutron::Access>>(
          proton::Nature::access_quill);
        factory->record<neutron::Access>(
          proton::Nature::access_value);

        factory->record<proton::Seam<neutron::Ensemble>>(
          proton::Nature::ensemble_seam);
        factory->record<proton::Quill<neutron::Ensemble>>(
          proton::Nature::ensemble_quill);
        factory->record<neutron::Ensemble>(
          proton::Nature::ensemble_value);

        return (factory);
      }
    }
  }

  namespace factory
  {
    /*----------.
    | Functions |
    `----------*/

    elle::utility::Factory<neutron::Component> const&
    block()
    {
      static elle::utility::Factory<neutron::Component> const* factory =
        setup::_block();

      ELLE_ASSERT(factory != nullptr);

      return (*factory);
    }

    elle::utility::Factory<proton::Nature> const&
    node()
    {
      static elle::utility::Factory<proton::Nature> const* factory =
        setup::_node();

      ELLE_ASSERT(factory != nullptr);

      return (*factory);
    }
  }
}
