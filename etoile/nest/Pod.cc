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
  elle::Object(element),

  nature(element.nature),
  state(element.state),
  placement(element.placement),
  address(element.address),
  block(element.block),
  counter(element.counter)
{
}

///
/// XXX
///
Pod::~Pod()
{
  if (this->block != nullptr)
    delete this->block;
}

//
// ---------- methods ---------------------------------------------------------
//

///
/// XXX
///
elle::Status            Pod::Load(nucleus::Handle&              handle)
{
  assert(this->nature != Pod::NatureOrphan);

  if (this->block != nullptr)
    handle.block = this->block;
  else
    {
      // XXX retrieve from network.
    }

  this->counter++;

  this->state = Pod::StateLoaded;

  return elle::Status::Ok;
}

///
/// XXX
///
elle::Status            Pod::Unload(nucleus::Handle&            handle)
{
  assert(this->counter > 0);

  handle.block = nullptr;

  this->counter--;

  if (this->counter == 0)
    this->state = Pod::StateUnloaded;

  return elle::Status::Ok;
}

//
// ---------- object ----------------------------------------------------------
//

///
/// this macro-function call generates the object.
///
embed(Pod, _());

//
// ---------- dumpable --------------------------------------------------------
//

///
/// this function dumps an object.
///
elle::Status            Pod::Dump(elle::Natural32               margin) const
{
  elle::String          alignment(margin, ' ');

  std::cout << alignment << "[Pod] " << std::hex << this << std::endl;

  std::cout << alignment << elle::Dumpable::Shift << "[Nature] "
            << std::dec << this->nature << std::endl;

  std::cout << alignment << elle::Dumpable::Shift << "[State] "
            << std::dec << this->state << std::endl;

  if (this->placement.Dump(margin + 2) == elle::Status::Error)
    escape("unable to dump the placement");

  if (this->address.Dump(margin + 2) == elle::Status::Error)
    escape("unable to dump the address");

  if (this->block != nullptr)
    {
      if (this->block->Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the block");
    }
  else
    {
      std::cout << alignment << elle::Dumpable::Shift << "[Block] "
                << elle::none << std::endl;
    }

  std::cout << alignment << elle::Dumpable::Shift << "[Counter] "
            << std::dec << this->counter << std::endl;

  return elle::Status::Ok;
}
