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
  _block(nullptr),
  _XXX(nullptr)
{
}

///
/// XXX
///
Handle::Handle(Block*                                           block):
  address(Address::Some),
  _block(nullptr), // XXX
  _XXX(block)
{
}

///
/// XXX
///
Handle::Handle(const Address&                                   address):
  address(address),
  _block(nullptr),
  _XXX(nullptr)
{
}

///
/// the copy constructor.
//
Handle::Handle(const Handle&                                    element):
  elle::Object(element),

  _placement(element._placement),
  address(element.address),
  _block(nullptr),
  _XXX(element._XXX)
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
  return (Porcupine<>::Load.Call(*this));
}

///
/// XXX
///
elle::Status            Handle::Unload()
{
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

  // XXX[ca c'est faux: si le deuxieme est pas loade, il sera null!]
  /* XXX
  // compare the objects.
  if ((this->_block != nullptr) || (element._block != nullptr))
    {
      if (this->_block != element._block)
        return elle::StatusFalse;
    }
  */

  // XXX
  if ((this->_XXX != nullptr) || (element._XXX != nullptr))
    {
      if (this->_XXX != element._XXX)
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
  if (this->_block != NULL)
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

  // XXX
  std::cout << alignment << elle::Dumpable::Shift
            << "[_XXX] " << std::hex << this->_XXX << std::endl;

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
