//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [thu mar 15 10:18:50 2012]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/nest/Pod.hh>

using namespace etoile::nest;

//
// ---------- constructors & destructors --------------------------------------
//

///
/// XXX
///
Pod::Pod():
  nature(NatureUnknown),
  state(StateUnloaded),
  block(nullptr),
  counter(0)
{
}

///
/// XXX
///
Pod::Pod(const nucleus::Placement&                              placement,
         nucleus::Block*                                        block):
  nature(NatureVolatile),
  state(StateUnloaded),
  placement(placement),
  block(block),
  counter(0)
{
}

///
/// XXX
///
Pod::Pod(const nucleus::Placement&                              placement,
         const nucleus::Address&                                address):
  nature(NaturePersistent),
  state(StateUnloaded),
  placement(placement),
  address(address),
  block(nullptr),
  counter(0)
{
}

///
/// XXX
///
Pod::Pod(const Pod&                                             element):
  nature(element.nature),
  state(element.state),
  placement(element.placement),
  address(element.address),
  block(element.block),
  counter(element.counter)
{
}

//
// ---------- object ----------------------------------------------------------
//

///
/// this macro-function call generates the object.
///
embed(Pod, _());
