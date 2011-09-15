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

    // disable the meta logging.
    if (elle::Meta::Disable() == elle::StatusError)
      escape("unable to disable the meta logging");

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

    // enable the meta logging.
    if (elle::Meta::Enable() == elle::StatusError)
      escape("unable to enable the meta logging");

    leave();
  }

  ///
  /// this method cleans the nucleus
  ///
  elle::Status		Nucleus::Clean()
  {
    enter();

    // nothing to do.

    leave();
  }

}
