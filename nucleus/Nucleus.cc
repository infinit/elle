//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [thu jan 28 22:01:03 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/Nucleus.hh>

namespace nucleus
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// the nucleus factory which enables one to generate nucleus instances
  /// based on its component identifier.
  ///
  elle::Factory			Nucleus::Factory;

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method initializes the nucleus by registering the component
  /// types to the factory.
  ///
  elle::Status		Nucleus::Initialize()
  {
    enter();

    //
    // register the nucleus products.
    //
    {
      // register the component types.
      if (Nucleus::Factory.Register< neutron::Object >
	  (neutron::ComponentObject) == elle::StatusError)
	escape("unable to register the factory product");

      if (Nucleus::Factory.Register< neutron::Contents<neutron::Data> >
	  (neutron::ComponentData) == elle::StatusError)
	escape("unable to register the factory product");

      if (Nucleus::Factory.Register< neutron::Contents<neutron::Catalog> >
	  (neutron::ComponentCatalog) == elle::StatusError)
	escape("unable to register the factory product");

      if (Nucleus::Factory.Register< neutron::Contents<neutron::Reference> >
	  (neutron::ComponentReference) == elle::StatusError)
	escape("unable to register the factory product");

      if (Nucleus::Factory.Register< neutron::Access >
	  (neutron::ComponentAccess) == elle::StatusError)
	escape("unable to register the factory product");

      if (Nucleus::Factory.Register< proton::Seam<neutron::Catalog> >
	  (neutron::ComponentSeamCatalog) == elle::StatusError)
	escape("unable to register the factory product");

      if (Nucleus::Factory.Register< proton::Quill<neutron::Catalog> >
	  (neutron::ComponentQuillCatalog) == elle::StatusError)
	escape("unable to register the factory product");

      // XXX same for data
    }

    // initialize the proton.
    if (proton::Proton::Initialize() == elle::StatusError)
      escape("unable to initialize the proton");

    leave();
  }

  ///
  /// this method cleans the nucleus
  ///
  elle::Status		Nucleus::Clean()
  {
    enter();

    // initialize the proton.
    if (proton::Proton::Initialize() == elle::StatusError)
      escape("unable to initialize the proton");

    // clear the nucleus factory.
    if (Nucleus::Factory.Clear() == elle::StatusError)
      escape("unable to clear the factory");

    leave();
  }

}
