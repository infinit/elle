//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [sat may 21 13:41:42 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/proton/Version.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this constant represents the first version.
    ///
    const Version               Version::First;

    ///
    /// this constant represents the latest version.
    ///
    const Version               Version::Last(
                                  elle::Type<Version::Type>::Maximum);

    ///
    /// this constant represents any version and is useful whenever
    /// dealing with immutable blocks for which version do not make any
    /// sense.
    ///
    const Version               Version::Any(Version::Last);

    ///
    /// this constant is an alias of Any.
    ///
    const Version&              Version::Some = Version::Any;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Version::Version():
      number(0)
    {
    }

    ///
    /// specific constructor.
    ///
    Version::Version(const Type                                 number):
      number(number)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a version.
    ///
    elle::Status        Version::Create(const Type              number)
    {
      enter();

      // assign the number.
      this->number = number;

      leave();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this method checks if two objects match.
    ///
    elle::Boolean       Version::operator==(const Version&      element) const
    {
      enter();

      // check the address as this may actually be the same object.
      if (this == &element)
        true();

      // compare the numbers.
      if (this->number != element.number)
        false();

      true();
    }

    ///
    /// this method compares the objects.
    ///
    elle::Boolean       Version::operator<(const Version&       element) const
    {
      enter();

      // check the address as this may actually be the same object.
      if (this == &element)
        true();

      // compare the numbers.
      if (this->number >= element.number)
        false();

      true();
    }

    ///
    /// this method compares the objects.
    ///
    elle::Boolean       Version::operator>(const Version&       element) const
    {
      enter();

      // check the address as this may actually be the same object.
      if (this == &element)
        true();

      // compare the numbers.
      if (this->number <= element.number)
        false();

      true();
    }

    ///
    /// this method increments the version number.
    ///
    Version&            Version::operator+=(const elle::Natural32 increment)
    {
      // increment the number.
      this->number += increment;

      return (*this);
    }

    ///
    /// this method adds the given version to the current one.
    ///
    Version             Version::operator+(const Version&       element) const
    {
      return (Version(this->number + element.number));
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Version, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the version's internals.
    ///
    elle::Status        Version::Dump(const elle::Natural32     margin) const
    {
      elle::String      alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Version] " << this->number << std::endl;

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the version attributes.
    ///
    elle::Status        Version::Serialize(elle::Archive&       archive) const
    {
      enter();

      // serialize the attributes.
      if (archive.Serialize(this->number) == elle::StatusError)
        escape("unable to serialize the version's attributes");

      leave();
    }

    ///
    /// this method extracts the attributes.
    ///
    elle::Status        Version::Extract(elle::Archive&         archive)
    {
      enter();

      // extracts the attributes.
      if (archive.Extract(this->number) == elle::StatusError)
        escape("unable to extract the version's attributes");

      leave();
    }

  }
}
