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

#include <elle/log.hh>

ELLE_LOG_COMPONENT("infinit.nucleus");

namespace nucleus
{

  /*----------.
  | Functions |
  `----------*/

  static
  elle::utility::Factory const*
  _setup()
  {
    elle::utility::Factory* factory = new elle::utility::Factory;

    ELLE_TRACE("setting up the nucleus factory");

    if (factory->Register< neutron::Object >
        (neutron::ComponentObject) == elle::Status::Error)
      throw Exception("unable to register the factory product");

    if (factory->Register< proton::Contents<neutron::Data> >
        (neutron::ComponentData) == elle::Status::Error)
      throw Exception("unable to register the factory product");

    if (factory->Register< proton::Contents<neutron::Catalog> >
        (neutron::ComponentCatalog) == elle::Status::Error)
      throw Exception("unable to register the factory product");

    if (factory->Register< proton::Contents<neutron::Reference> >
        (neutron::ComponentReference) == elle::Status::Error)
      throw Exception("unable to register the factory product");

    if (factory->Register< neutron::Access >
        (neutron::ComponentAccess) == elle::Status::Error)
      throw Exception("unable to register the factory product");

    if (factory->Register< neutron::Group >
        (neutron::ComponentGroup) == elle::Status::Error)
      throw Exception("unable to register the factory product");

    if (factory->Register< neutron::Ensemble >
        (neutron::ComponentEnsemble) == elle::Status::Error)
      throw Exception("unable to register the factory product");

    return (factory);
  }

  elle::utility::Factory const&
  factory()
  {
    static elle::utility::Factory const* factory = _setup();

    assert(factory != nullptr);

    return (*factory);
  }

}
