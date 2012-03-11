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
  _block(nullptr)
{
}

///
/// XXX
///
Handle::Handle(Block*                                           block):
address(Address::Some), // XXX[???]
_block(block) // XXX
{
}

///
/// XXX
///
Handle::Handle(const Address&                                   address):
address(address),
_block(nullptr)
{
}

/* XXX
   XXX
   - on load un block.
   - on veut loader son frere gauche, on load a partir de l addresse.
   - le nest regarde si il a un mapping entre cette adresse et un block
   deja loade mais qui a ete modifie. si cest le cas, le mapping nous donne
   le placement du block et on le recupere.
   - sinon on recupere a partir de hole.

   - en soit on a 3 cas:
   - si le handle a un pointer, on l utilise.
   - si le handle a un placement, on recupere le block du nest
   - sinon, on prend l address, et on load a partir du nest (mais le
   mapping pourrait nous retourner un block deja loade)

   - a noter que peut etre qu il serait utile de garder le handle dans chaque
   nodule pour eviter de recalculer.
*/

///
/// the copy constructor.
//
Handle::Handle(const Handle&                                    element):
  elle::Object(element),

  address(element.address),
  _block(element._block),
  _placement(element._placement)
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
elle::Status        Handle::Create(const Address&               address,
                                   Block*                       block)
{
  // set the attributes.
  this->address = address;
  this->_block = block;

  return elle::StatusOk;
}

//
// ---------- object ----------------------------------------------------------
//

///
/// this operator compares two objects.
///
elle::Boolean       Handle::operator==(const Handle&            element) const
{
  // check the address as this may actually be the same object.
  if (this == &element)
    return elle::StatusTrue;

  // compare the objects.
  if ((this->_block != nullptr) || (element._block != nullptr))
    {
      if (this->_block != element._block)
        return elle::StatusFalse;
    }

  // XXX

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
elle::Status        Handle::Dump(const elle::Natural32          margin) const
{
  elle::String      alignment(margin, ' ');

  std::cout << alignment << "[Handle]" << std::endl;

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
                << elle::Dumpable::Shift
                << "[_Block] " << elle::none << std::endl;
    }

  // dump the placement.
  if (this->_placement.Dump(margin + 2) == elle::StatusError)
    escape("unable to dump the placement");

  return elle::StatusOk;
}

//
// ---------- archivable ------------------------------------------------------
//

///
/// this method serializes the object.
///
elle::Status        Handle::Serialize(elle::Archive&            archive) const
{
  if (archive.Serialize(this->address) == elle::StatusError)
    escape("unable to serialize the attribtues");

  return elle::StatusOk;
}

///
/// this method extracts the object.
///
elle::Status        Handle::Extract(elle::Archive&              archive)
{
  if (archive.Extract(this->address) == elle::StatusError)
    escape("unable to extract the attribtues");

  return elle::StatusOk;
}
