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

#include <Infinit.hh>

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
/// note that the default secret is used so as to represent a
/// valid secret key for the footprint computation.
///
Handle::Handle():
  state(StateUnloaded),
  secret(Porcupine<>::Default::Secret),
  block(nullptr)
{
}

///
/// XXX on met address to some pour que le fingerprint nous sorte une
///     addresse de taille commune i.e 20 octets environ. avec null on
///     aurait 1.
///
    /// note that the default secret is used so as to represent a
    /// valid secret key for the footprint computation.
    ///
Handle::Handle(Placement&                                       placement):
  state(StateUnloaded),
  placement(placement),
  address(Address::Some),
  secret(Porcupine<>::Default::Secret),
  block(nullptr)
{
}

///
/// XXX
///
    /// note that the default secret is used so as to represent a
    /// valid secret key for the footprint computation.
    ///
Handle::Handle(const Address&                                   address):
  state(StateUnloaded),
  address(address),
  secret(Porcupine<>::Default::Secret),
  block(nullptr)
{
}

///
/// the copy constructor.
///
/// note that the block is not retrieved from the element in order
/// to force the cloned handle to be loaded before being used.
///
Handle::Handle(const Handle&                                    element):
  elle::radix::Object(element),

  state(StateUnloaded),
  placement(element.placement),
  address(element.address),
  secret(element.secret),
  block(nullptr)
{
}

//
// ---------- methods ---------------------------------------------------------
//

///
/// XXX
///
elle::Status            Handle::Load()
{
  // debug.
  if (Infinit::Configuration.nucleus.debug == true)
    printf("[nucleus] proton::Handle::Load()\n");

  if (this->state == Handle::StateLoaded)
    escape("unable to load an already loaded block");

  assert(this->block == nullptr);

  if (Porcupine<>::Load.Call(*this) == elle::Status::Error)
    escape("unable to load the block");

  this->state = Handle::StateLoaded;

  return elle::Status::Ok;
}

///
/// XXX
///
elle::Status            Handle::Unload()
{
  // debug.
  if (Infinit::Configuration.nucleus.debug == true)
    printf("[nucleus] proton::Handle::Unload()\n");

  if (this->state == Handle::StateUnloaded)
    escape("unable to unload an already unloaded block");

  assert(this->block != nullptr);

  if (Porcupine<>::Unload.Call(*this) == elle::Status::Error)
    escape("unable to unload the block");

  this->state = Handle::StateUnloaded;

  return elle::Status::Ok;
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
    return elle::Status::True;

  // compare the placements, if possible.
  if ((this->placement != Placement::Null) &&
      (element.placement != Placement::Null))
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

      if (this->placement != element.placement)
        return elle::Status::False;
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
        return elle::Status::False;
    }

  return elle::Status::True;
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

  // dump the state.
  std::cout << alignment << elle::Dumpable::Shift
            << "[State] " << std::dec << this->state << std::endl;

  // dump the placement.
  if (this->placement.Dump(margin + 2) == elle::Status::Error)
    escape("unable to dump the placement");

  // dump the address.
  if (this->address.Dump(margin + 2) == elle::Status::Error)
    escape("unable to dump the address");

  // dump the secret.
  if (this->secret.Dump(margin + 2) == elle::Status::Error)
    escape("unable to dump the secret");

  // dump the block.
  if (this->block != nullptr)
    {
      // dump the hierarchy, if present.
      std::cout << alignment << elle::Dumpable::Shift
                << "[Block]" << std::endl;

      if (this->block->Dump(margin + 4) == elle::Status::Error)
        escape("unable to dump the nodule");
    }
  else
    {
      std::cout << alignment << elle::Dumpable::Shift
                << "[Block] " << elle::none << std::endl;
    }

  return elle::Status::Ok;
}

//
// ---------- archivable ------------------------------------------------------
//

///
/// this method serializes the object.
///
//elle::Status            Handle::Serialize(elle::Archive&        archive) const
//{
//  if (archive.Serialize(this->address,
//                        this->secret) == elle::Status::Error)
//    escape("unable to serialize the attribtues");
//
//  return elle::Status::Ok;
//}
//
/////
///// this method extracts the object.
/////
//elle::Status            Handle::Extract(elle::Archive&          archive)
//{
//  if (archive.Extract(this->address,
//                      this->secret) == elle::Status::Error)
//    escape("unable to extract the attribtues");
//
//  return elle::Status::Ok;
//}
