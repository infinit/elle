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

  /*----------.
  | Functions |
  `----------*/

  static
  elle::utility::Factory<neutron::Component> const*
  _setup()
  {
    elle::utility::Factory<neutron::Component>* factory =
      new elle::utility::Factory<neutron::Component>;

    ELLE_TRACE("setting up the nucleus factory");

    factory->record<neutron::Object>(neutron::ComponentObject);
    factory->record<proton::Contents<neutron::Data>>(
      neutron::ComponentData);
    factory->record<proton::Contents<neutron::Catalog>>(
      neutron::ComponentCatalog);
    factory->record<proton::Contents<neutron::Reference>>(
      neutron::ComponentReference);
    // XXX factory->record<proton::Contents>(
    //       neutron::ComponentContents); // XXX[should be in neutron?]
    factory->record<neutron::Access>(neutron::ComponentAccess);
    factory->record<neutron::Group>(neutron::ComponentGroup);
    factory->record<neutron::Ensemble>(neutron::ComponentEnsemble);

    return (factory);
  }

  elle::utility::Factory<neutron::Component> const&
  factory()
  {
    static elle::utility::Factory<neutron::Component> const* factory = _setup();

    ELLE_ASSERT(factory != nullptr);

    return (*factory);
  }

}
