
#include <elle/serialize/BinaryArchive.hh>

#include <nucleus/proton/BlockSerializer.hxx>
#include <nucleus/Nucleus.hh>

#include <hole/Hole.hh>

namespace nucleus
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// the nucleus factory which enables one to generate nucleus instances
  /// based on its component identifier.
  ///
  elle::Factory                 Nucleus::Factory;

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
    }

    // initialize the proton.
    if (proton::Proton::Initialize() == elle::Status::Error)
      escape("unable to initialize the proton");

    return elle::Status::Ok;
  }

  ///
  /// this method cleans the nucleus
  ///
  elle::Status          Nucleus::Clean()
  {
    // initialize the proton.
    if (proton::Proton::Initialize() == elle::Status::Error)
      escape("unable to initialize the proton");

    // clear the nucleus factory.
    if (Nucleus::Factory.Clear() == elle::Status::Error)
      escape("unable to clear the factory");

    return elle::Status::Ok;
  }

}
