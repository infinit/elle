//
// ---------- header ----------------------------------------------------------
//
// project       XXX
//
// license       infinit
//
// author        julien quintard   [sun mar 11 14:53:13 2012]
//

//
// ---------- includes --------------------------------------------------------
//

#include <XXX/Handle.hh>
#include <XXX/Porcupine.hh>

using namespace nucleus::proton;

//
// ---------- definitions -----------------------------------------------------
//

///
/// XXX
///
const Handle                     Handle::Null;

//
// ---------- constructors & destructors --------------------------------------
//

///
/// default constructor.
///
Handle::Handle():
  state(StateUnloaded),
  _block(nullptr)
{
}

///
/// XXX
///
Handle::Handle(Placement&                                       placement):
  state(StateUnloaded),
  _placement(placement),
  address(Address::Some),
  _block(nullptr)
{
}

///
/// XXX
///
Handle::Handle(const Address&                                   address):
  state(StateUnloaded),
  address(address),
  _block(nullptr)
{
}

///
/// the copy constructor.
///
/// note that the block is not retrieved from the element in order
/// to force the cloned handle to be loaded before being used.
///
Handle::Handle(const Handle&                                    element):
  elle::Object(element),

  state(StateUnloaded),
  _placement(element._placement),
  address(element.address),
  _block(nullptr)
{
}

///
/// the destructor.
///
Handle::~Handle()
{
  // do nothing.
}

//
// ---------- methods ---------------------------------------------------------
//

///
/// XXX
///
elle::Status            Handle::Load()
{
  if (this->state == Handle::StateLoaded)
    escape("unable to load an already loaded block");

  assert(this->_block == nullptr);

  return (Porcupine<>::Load.Call(*this));
}

///
/// XXX
///
elle::Status            Handle::Unload()
{
  if (this->state == Handle::StateUnloaded)
    escape("unable to unloaded an already unloaded block");

  assert(this->_block != nullptr);

  return (Porcupine<>::Unload.Call(*this));
}

//
// ---------- object ----------------------------------------------------------
//

///
/// this operator compares two objects.
///
elle::Boolean           Handle::operator==(const Handle&        element) const
{
  // check the address as this may actually be the same object.
  if (this == &element)
    return elle::StatusTrue;

  // compare the placements, if possible.
  if ((this->_placement != Placement::Null) &&
      (element._placement != Placement::Null))
    {
      //
      // in this case, both handles reference blocks which have been
      // retrieved from the network.
      //
      // however, that does not necessarily mean that the blocks have
      // been loaded in main memory. therefore, the block addresses
      // cannot be used for comparing the blocks.
      //
      // thus, the placements are compared as representing unique
      // identifiers.
      //

      if (this->_placement != element._placement)
        return elle::StatusFalse;
    }
  else
    {
      //
      // otherwise, one of the two handles has not been retrieved from
      // the network.
      //
      // if both blocks have been created, they would both have had
      // a placement. as a result, one or more of the handles represent
      // a block which resides on the network storage layer.
      //
      // since a created block would have a special address (i.e
      // Address::Some) which would differ from normal addresses,
      // one can rely on the address field for comparing the handles.
      //
      // in other words, addresses could be null in which case,
      // if both are, they would be equal and everything would be fine.
      //
      // if both addresses are set to 'some', then, the handles would
      // represent created blocks in which case both would have a placement
      // and we would not be discussing in the branch of the condition.
      //
      // finally, if one handle is null or 'some' and the other one is
      // not, then, the comparison will result in handles being different
      // which they are.
      //

      if (this->address != element.address)
        return elle::StatusFalse;
    }

  return elle::StatusTrue;
}

///
/// this macro-function call generates the object.
///
embed(Handle, _());

//
// ---------- dumpable --------------------------------------------------------
//

///
/// XXX
///
elle::Status            Handle::Dump(const elle::Natural32      margin) const
{
  elle::String          alignment(margin, ' ');

  std::cout << alignment << "[Handle]" << std::endl;

  // dump the placement.
  if (this->_placement.Dump(margin + 2) == elle::StatusError)
    escape("unable to dump the placement");

  // dump the address.
  if (this->address.Dump(margin + 2) == elle::StatusError)
    escape("unable to dump the address");

  // dump the block.
  if (this->_block != nullptr)
    {
      // dump the hierarchy, if present.
      std::cout << alignment << elle::Dumpable::Shift
                << "[_Block]" << std::endl;

      if (this->_block->Dump(margin + 4) == elle::StatusError)
        escape("unable to dump the nodule");
    }
  else
    {
      std::cout << alignment << elle::Dumpable::Shift
                << "[_Block] " << elle::none << std::endl;
    }

  return elle::StatusOk;
}

//
// ---------- archivable ------------------------------------------------------
//

///
/// this method serializes the object.
///
elle::Status            Handle::Serialize(elle::Archive&        archive) const
{
  if (archive.Serialize(this->address) == elle::StatusError)
    escape("unable to serialize the attribtues");

  return elle::StatusOk;
}

///
/// this method extracts the object.
///
elle::Status            Handle::Extract(elle::Archive&          archive)
{
  if (archive.Extract(this->address) == elle::StatusError)
    escape("unable to extract the attribtues");

  return elle::StatusOk;
}
