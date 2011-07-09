//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/Nucleus.cc
//
// created       julien quintard   [thu jan 28 22:01:03 2010]
// updated       julien quintard   [sat jul  9 19:46:59 2011]
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

    leave();
  }

  ///
  /// this method cleans the nucleus
  ///
  /// the components are recycled just to make sure the memory is
  /// released before the Meta allocator terminates.
  ///
  elle::Status		Nucleus::Clean()
  {
    enter();

    // recycle the factory.
    if (Nucleus::Factory.Recycle<elle::Factory>() == elle::StatusError)
      escape("unable to recycle the factory");

    leave();
  }

}
