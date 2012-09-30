#include <nucleus/Nucleus.hh>
#include <nucleus/proton/Contents.hh>
#include <nucleus/neutron/Object.hh>
#include <nucleus/neutron/Data.hh>
#include <nucleus/neutron/Catalog.hh>
#include <nucleus/neutron/Reference.hh>
#include <nucleus/neutron/Access.hh>
#include <nucleus/neutron/Group.hh>
#include <nucleus/neutron/Ensemble.hh>

namespace nucleus
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// the nucleus factory which enables one to generate nucleus instances
  /// based on its component identifier.
  ///
  elle::utility::Factory                 Nucleus::Factory;

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method initializes the nucleus by registering the component
  /// types to the factory.
  ///
  elle::Status          Nucleus::Initialize()
  {
    //
    // register the nucleus products.
    //
    {
      // register the component types.
      if (Nucleus::Factory.Register< neutron::Object >
          (neutron::ComponentObject) == elle::Status::Error)
        escape("unable to register the factory product");

      if (Nucleus::Factory.Register< proton::Contents<neutron::Data> >
          (neutron::ComponentData) == elle::Status::Error)
        escape("unable to register the factory product");

      if (Nucleus::Factory.Register< proton::Contents<neutron::Catalog> >
          (neutron::ComponentCatalog) == elle::Status::Error)
        escape("unable to register the factory product");

      if (Nucleus::Factory.Register< proton::Contents<neutron::Reference> >
          (neutron::ComponentReference) == elle::Status::Error)
        escape("unable to register the factory product");

      if (Nucleus::Factory.Register< neutron::Access >
          (neutron::ComponentAccess) == elle::Status::Error)
        escape("unable to register the factory product");

      if (Nucleus::Factory.Register< neutron::Group >
          (neutron::ComponentGroup) == elle::Status::Error)
        escape("unable to register the factory product");

      if (Nucleus::Factory.Register< neutron::Ensemble >
          (neutron::ComponentEnsemble) == elle::Status::Error)
        escape("unable to register the factory product");
    }

    return elle::Status::Ok;
  }

  ///
  /// this method cleans the nucleus
  ///
  elle::Status          Nucleus::Clean()
  {
    // clear the nucleus factory.
    if (Nucleus::Factory.Clear() == elle::Status::Error)
      escape("unable to clear the factory");

    return elle::Status::Ok;
  }

}
