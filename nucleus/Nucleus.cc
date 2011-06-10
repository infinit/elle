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
// updated       julien quintard   [fri jun 10 13:09:05 2011]
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
    Nucleus::Factory.Register< neutron::Object >
      (neutron::ComponentObject);
    Nucleus::Factory.Register< neutron::Contents<neutron::Data> >
      (neutron::ComponentData);
    Nucleus::Factory.Register< neutron::Contents<neutron::Catalog> >
      (neutron::ComponentCatalog);
    Nucleus::Factory.Register< neutron::Contents<neutron::Reference> >
      (neutron::ComponentReference);
    Nucleus::Factory.Register< neutron::Access >
      (neutron::ComponentAccess);

    leave();
  }

  ///
  /// this method cleans the nucleus
  ///
  elle::Status		Nucleus::Clean()
  {
    enter();

    // XXX

    leave();
  }

}
