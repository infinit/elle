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
/// this variable holds the current placement counter.
///
const elle::Natural64               Placement::Zero = 0;

///
/// this variable defines an unused hence null Placement.
///
const Placement                     Placement::Null;

///
/// this variable holds the current placement counter.
///
elle::Natural64                     Placement::Counter =
  Placement::Zero + 1;

//
// ---------- constructors & destructors --------------------------------------
//

///
/// default constructor.
///
Placement::Placement():
  value(Placement::Zero)
{
}

//
// ---------- object ----------------------------------------------------------
//

///
/// this method check if two placements match.
///
elle::Boolean       Placement::operator==(const Placement& element)
  const
{
  // check the address as this may actually be the same object.
  if (this == &element)
    return elle::Status::True;

  // compare the placement.
  if (this->value != element.value)
    return elle::Status::False;

  return elle::Status::True;
}

///
/// this operator compares two placements.
///
elle::Boolean       Placement::operator<(const Placement&   element) const
{
  return (this->value < element.value);
}

///
/// this macro-function call generates the object.
///
embed(Placement, _());

//
// ---------- archivable ------------------------------------------------------
//

///
/// this method serializes the placement.
///
//elle::Status        Placement::Serialize(elle::Archive&     archive) const
//{
//  // serialize the attributes.
//  if (archive.Serialize(this->value) == elle::Status::Error)
//    escape("unable to serialize the placement attributes");
//
//  return elle::Status::Ok;
//};
//
/////
///// this method extracts the placement.
/////
//elle::Status        Placement::Extract(elle::Archive&       archive)
//{
//  // extract the attributes.
//  if (archive.Extract(this->value) == elle::Status::Error)
//    escape("unable to extract the placement attributes");
//
//  return elle::Status::Ok;
//};

//
// ---------- dumpable --------------------------------------------------------
//

///
/// this method dumps an placement.
///
elle::Status        Placement::Dump(const elle::Natural32   margin) const
{
  elle::String      alignment(margin, ' ');

  std::cout << alignment << "[Placement] "
            << std::dec << this->value << std::endl;

  return elle::Status::Ok;
}

//
// ---------- static methods --------------------------------------------------
//

///
/// this method generates a new unique placement.
///
elle::Status        Placement::Generate(Placement&              placement)
{
  // increments the counter.
  placement.value = Placement::Counter++;

  return elle::Status::Ok;
}
