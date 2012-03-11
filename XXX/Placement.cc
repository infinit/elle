//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [sat mar 10 22:05:39 2012]
//

//
// ---------- includes --------------------------------------------------------
//

#include <XXX/Placement.hh>

using namespace nucleus::proton;

//
// ---------- definitions -----------------------------------------------------
//

///
/// XXX
///
const Placement                 Placement::Null;

//
// ---------- constructors & destructors --------------------------------------
//

///
/// XXX
///
Placement::Placement():
  number(0)
{
}

//
// ---------- object ----------------------------------------------------------
//

///
/// this operator compares two objects.
///
elle::Boolean       Placement::operator==(const Placement&  element) const
{
  // check the placement as this may actually be the same object.
  if (this == &element)
    return elle::StatusTrue;

  // compare the numbers.
  if (this->number != element.number)
    return elle::StatusFalse;

  return elle::StatusTrue;
}

///
/// this macro-function call generates the object.
///
embed(Placement, _());

//
// ---------- dumpable --------------------------------------------------------
//

///
/// XXX
///
elle::Status        Placement::Dump(const elle::Natural32       margin) const
{
  elle::String      alignment(margin, ' ');

  std::cout << alignment << "[Placement] "
            << std::dec << this->number << std::endl;

  return elle::StatusOk;
}
