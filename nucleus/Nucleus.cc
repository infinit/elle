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
// updated       julien quintard   [sun may  8 09:01:28 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/Nucleus.hh>

namespace nucleus
{

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
    /* XXX
    elle::Factory::Register< neutron::Object >
      (neutron::Object::Name);
    elle::Factory::Register< neutron::Contents<neutron::Data> >
      (neutron::Contents<neutron::Data>::Name);
    elle::Factory::Register< neutron::Contents<neutron::Catalog> >
      (neutron::Contents<neutron::Catalog>::Name);
    elle::Factory::Register< neutron::Contents<neutron::Reference> >
      (neutron::Contents<neutron::Reference>::Name);
    */

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
